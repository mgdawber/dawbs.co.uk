package main

import (
	"net/http"
	"html/template"
	"fmt"
)

type M map[string]interface{}

func main() {
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/static/", http.StripPrefix("/static/", fs))

	var tmpl, err = template.ParseGlob("web/*")
	if err != nil {
		panic(err.Error())
	}

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		var data = M{"name": "batman"}
		err = tmpl.ExecuteTemplate(w, "index", data)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
		}
	})

	http.HandleFunc("/posts", func(w http.ResponseWriter, r *http.Request) {
		var data = M{"name": "batman"}
		err = tmpl.ExecuteTemplate(w, "posts", data)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
		}
	})

	http.HandleFunc("/illustrations", func(w http.ResponseWriter, r *http.Request) {
		var data = M{"name": "batman"}
		err = tmpl.ExecuteTemplate(w, "illustrations", data)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
		}
	})

	fmt.Println("server started at localhost:3000")
	http.ListenAndServe(":3000", nil)
}
