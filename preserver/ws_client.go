package main

import (
	"bytes"
	"encoding/json"
	"image"
	"image/jpeg"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

type FaceTimestamps struct {
	FirstSeen time.Time
	LastSeen  time.Time
}

type Face struct {
	ID    int    `json:"id"`
	Label string `json:"label"`
	X     int    `json:"x"`
	Y     int    `json:"y"`
	W     int    `json:"w"`
	H     int    `json:"h"`
}

type FaceMessage struct {
	Faces []Face `json:"faces"`
}

var (
	knownFaces  = make(map[int]*FaceTimestamps) // id -> timestamps
	faceTimeout = 1 * time.Second
	connTimeout = 5 * time.Second
	frameMu     sync.RWMutex
	latestFrame image.Image
)

func connectWS(url string, msgChan chan<- []byte) {
	for {
		c, _, err := websocket.DefaultDialer.Dial(url, nil)
		if err != nil {
			Logger.Errorf("connecting %s: %v", url, err)
			time.Sleep(connTimeout)
			continue
		}
		defer c.Close()

		for {
			_, data, err := c.ReadMessage()
			if websocket.IsCloseError(err) {
				break
			} else if err != nil {
				Logger.Errorf("read: %v", err)
				break
			}

			msgChan <- data
		}
	}
}

func handleStreamMessage(data []byte) error {
	img, err := jpeg.Decode(bytes.NewReader(data))
	if err != nil {
		return err
	}
	frameMu.Lock()
	latestFrame = img
	frameMu.Unlock()
	return nil
}

func handleFaceMessage(data []byte) error {
	var msg FaceMessage
	if err := json.Unmarshal(data, &msg); err != nil {
		return err
	}

	frameMu.RLock()
	img := latestFrame
	frameMu.RUnlock()
	if img == nil {
		return nil
	}

	now := time.Now()

	for _, face := range msg.Faces {
		id := face.ID

		_, exists := knownFaces[id]
		if exists {
			knownFaces[id].LastSeen = now // update LastSeen timestamp
			continue
		}

		rect := image.Rect(face.X, face.Y, face.X+face.W, face.Y+face.H)
		cropped := cropImage(img, rect)

		var buf bytes.Buffer
		jpeg.Encode(&buf, cropped, nil)
		Logger.Info("new face detected", "id", face.ID, "label", face.Label)

		faceCrop := &FaceCrop{
			FaceID:    face.ID,
			FirstSeen: now.Unix(),
			Label:     face.Label,
			Image:     buf.Bytes(),
		}
		if err := insertFaceCrop(faceCrop); err != nil {
			Logger.Errorf("failed to insert face crop: %v", err)
		}

		knownFaces[id] = &FaceTimestamps{
			FirstSeen: now,
			LastSeen:  now,
		}
	}

	for id, ts := range knownFaces {
		if now.Sub(ts.LastSeen) > faceTimeout {
			Logger.Info("seems to went away", "id", id)

			if err := updateFaceLastSeen(id, ts.FirstSeen.Unix(), now.Unix()); err != nil {
				Logger.Errorf("update face LastSeen: %v", err)
			}

			delete(knownFaces, id)
		}
	}
	return nil
}

func StartWSConnections(endpointURL string) {
	streamChan := make(chan []byte, 5)
	faceChan := make(chan []byte, 5)

	go connectWS(endpointURL+"/stream", streamChan)
	go connectWS(endpointURL+"/face", faceChan)

	go func() {
		for {
			select {
			case data := <-streamChan:
				if err := handleStreamMessage(data); err != nil {
					Logger.Errorf("stream message: %v", err)
				}
			case data := <-faceChan:
				if err := handleFaceMessage(data); err != nil {
					Logger.Errorf("face message: %v", err)
				}
			}
		}
	}()
}
