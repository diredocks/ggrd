package main

import (
	"os"

	"github.com/charmbracelet/lipgloss"
	"github.com/charmbracelet/log"
)

var (
	Logger = log.NewWithOptions(os.Stderr, log.Options{
		ReportTimestamp: true,
		TimeFormat:      "[06-01-02 15:04:05]",
	})
)

func SetLogger() {
	styles := log.DefaultStyles()
	styles.Levels[log.InfoLevel] = lipgloss.NewStyle().
		SetString("[I]").
		Foreground(lipgloss.Color("86"))
	styles.Levels[log.WarnLevel] = lipgloss.NewStyle().
		SetString("[W]").
		Foreground(lipgloss.Color("192"))
	styles.Levels[log.ErrorLevel] = lipgloss.NewStyle().
		SetString("[E]").
		Foreground(lipgloss.Color("204"))
	styles.Levels[log.FatalLevel] = lipgloss.NewStyle().
		SetString("[F]").
		Foreground(lipgloss.Color("134"))
	Logger.SetStyles(styles)
}

