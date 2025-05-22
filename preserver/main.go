package main

import (
	"fmt"
	"net/http"
)

func main() {
	SetLogger()
	SetRoutes()

	// Connect to WebSocket streaming servers
	go StartWSConnections("ws://localhost:9001")

	fmt.Println(`
                         .___
   ____   ___________  __| _/
  / ___\ / ___\_  __ \/ __ | 
 / /_/  > /_/  >  | \/ /_/ | 
 \___  /\___  /|__|  \____ | 
/_____//_____/            \/ [p]
	`)

	Logger.Info("preserver listening on :8080")
	Logger.Fatal(http.ListenAndServe(":8080", nil))
}
