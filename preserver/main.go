package main

import (
	"fmt"
	"log"
	"net/http"
)

func main() {

	SetRoutes()

	// Connect to WebSocket streaming servers
	go StartWSConnections("ws://localhost:9001")

	fmt.Println(`
                         .___
   ____   ___________  __| _/
  / ___\ / ___\_  __ \/ __ | 
 / /_/  > /_/  >  | \/ /_/ | 
 \___  /\___  /|__|  \____ | 
/_____//_____/            \/ 
	`)

	log.Println("API and frontend server listening on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
