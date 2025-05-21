package main

import (
	"image"
)

func cropImage(src image.Image, rect image.Rectangle) image.Image {
	// Clamp to bounds
	bounds := src.Bounds().Intersect(rect)
	cropped := image.NewRGBA(bounds)
	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			cropped.Set(x, y, src.At(x, y))
		}
	}
	return cropped
}
