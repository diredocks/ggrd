package main

import (
	"ggrd-preserver/frontend"
	"io"
	"net/http"
)

func SetRoutes() {
	// Route for your face API
	http.HandleFunc("/face", func(w http.ResponseWriter, r *http.Request) {
		io.WriteString(w, "Hello from a HandleFunc #1!\n")
	})
	// Serve embedded files at "/"
	fileServer := http.FileServer(http.FS(frontend.FS))
	http.Handle("/", fileServer)
}
