package models

import "gorm.io/gorm"

type Post struct {
    gorm.Model  // adds ID, created_at etc.
    Title       string `json:"title"`
	Content		string `json:"content"`
}
