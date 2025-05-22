package main

import (
	"database/sql"

	_ "github.com/mattn/go-sqlite3"
)

var db *sql.DB

type FaceCrop struct {
	ID        int    `json:"-" db:"id"`       // internal auto-incrementing ID
	FaceID    int    `json:"id" db:"face_id"` // external face ID from detection
	FirstSeen int64  `json:"first_seen" db:"first_seen"`
	LastSeen  int64  `json:"last_seen" db:"last_seen"`
	Label     string `json:"label" db:"label"`
	Image     []byte `json:"-" db:"image"`
}

func initDB() error {
	var err error
	db, err = sql.Open("sqlite3", "./faces.db")
	if err != nil {
		return err
	}

	// create table if it doesn't exist
	_, err = db.Exec(`
		CREATE TABLE IF NOT EXISTS face_crops (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			face_id INTEGER NOT NULL,
			first_seen INTEGER NOT NULL,
			last_seen INTEGER,
			label TEXT NOT NULL,
			image BLOB NOT NULL
		)
	`)
	return err
}

func insertFaceCrop(face *FaceCrop) error {
	_, err := db.Exec(
		`INSERT INTO face_crops
		(face_id, first_seen, last_seen, label, image)
		VALUES (?, ?, NULL, ?, ?)`,
		face.FaceID, face.FirstSeen, face.Label, face.Image,
	)
	return err
}

func updateFaceLastSeen(faceID int, firstSeen int64, lastSeen int64) error {
	_, err := db.Exec(
		`UPDATE face_crops SET last_seen = ?
		WHERE face_id = ? AND first_seen = ? AND last_seen IS NULL`,
		lastSeen, faceID, firstSeen,
	)
	return err
}
