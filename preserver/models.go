package main

type FaceCrop struct {
	ID        int    `json:"id"`
	Timestamp int64  `json:"timestamp"`
	Label     string `json:"label"`
	Image     []byte `json:"-"`
}
