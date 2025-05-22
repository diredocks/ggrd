package main

import (
	"encoding/json"
	"ggrd-preserver/frontend"
	"net/http"
	"strconv"
)

func handleFaceAPI(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	// parse optional query parameters
	var timeFilter *int64
	var labelFilter *string

	if timeStr := r.URL.Query().Get("time"); timeStr != "" {
		t, err := strconv.ParseInt(timeStr, 10, 64)
		if err != nil {
			http.Error(w, "Invalid time parameter", http.StatusBadRequest)
			return
		}
		timeFilter = &t
	}

	if label := r.URL.Query().Get("label"); label != "" {
		labelFilter = &label
	}

	faces, err := getFaceCrops(timeFilter, labelFilter)
	if err != nil {
		Logger.Error("getting crops", "err", err)
		http.Error(w, "Internal server error", http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	// encode and send response
	if err := json.NewEncoder(w).Encode(faces); err != nil {
		http.Error(w, "Error encoding response", http.StatusInternalServerError)
		return
	}
}

func handleImageAPI(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	idStr := r.URL.Query().Get("id")
	if idStr == "" {
		http.Error(w, "Missing id parameter", http.StatusBadRequest)
		return
	}
	id, err := strconv.Atoi(idStr)
	if err != nil {
		http.Error(w, "Invalid id parameter", http.StatusBadRequest)
		return
	}

	face, err := getFaceCropByID(id)
	if err != nil {
		Logger.Error("getting face crop", "err", err)
		http.Error(w, "Internal server error", http.StatusInternalServerError)
		return
	}
	if face == nil {
		http.Error(w, "Image not found", http.StatusNotFound)
		return
	}

	// send image data
	w.Header().Set("Content-Type", "image/jpeg")
	if _, err := w.Write(face.Image); err != nil {
		Logger.Error("writing image response", "err", err)
		http.Error(w, "Error sending image", http.StatusInternalServerError)
		return
	}
}

func SetRoutes() {
	// route for face API
	http.HandleFunc("/api/face", handleFaceAPI)
	// route for image API
	http.HandleFunc("/api/image", handleImageAPI)

	// serve embedded files at "/"
	fileServer := http.FileServer(http.FS(frontend.FS))
	http.Handle("/", fileServer)
}
