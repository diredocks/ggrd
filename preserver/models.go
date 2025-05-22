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
			last_seen INTEGER NOT NULL,
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
		VALUES (?, ?, 0, ?, ?)`,
		face.FaceID, face.FirstSeen, face.Label, face.Image,
	)
	return err
}

func updateFaceLastSeen(faceID int, firstSeen int64, lastSeen int64) error {
	_, err := db.Exec(
		`UPDATE face_crops SET last_seen = ?
		WHERE face_id = ? AND first_seen = ? AND last_seen IS 0`,
		lastSeen, faceID, firstSeen,
	)
	return err
}

func getFaceCrops(timeFilter *int64, labelFilter *string) ([]FaceCrop, error) {
	query := `SELECT id, face_id, first_seen, last_seen, label FROM face_crops WHERE 1=1`
	args := []any{}

	if timeFilter != nil {
		query += ` AND (first_seen <= ? OR last_seen >= ?)`
		args = append(args, *timeFilter, *timeFilter)
	}

	if labelFilter != nil {
		query += ` AND label = ?`
		args = append(args, *labelFilter)
	}

	query += ` ORDER BY first_seen DESC`

	rows, err := db.Query(query, args...)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var faces []FaceCrop
	for rows.Next() {
		var face FaceCrop
		err := rows.Scan(&face.ID, &face.FaceID, &face.FirstSeen, &face.LastSeen, &face.Label)
		if err != nil {
			return nil, err
		}
		faces = append(faces, face)
	}
	return faces, nil
}

func getFaceCropByID(id int) (*FaceCrop, error) {
	var face FaceCrop
	err := db.QueryRow(
		`SELECT id, image 
		FROM face_crops WHERE id = ?`,
		id,
	).Scan(&face.ID, &face.Image)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}
	return &face, nil
}
