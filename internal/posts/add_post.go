package posts

import (
    "net/http"

    "github.com/gin-gonic/gin"
    "github.com/mgdawber/dawbs-api/internal/common/models"
)

type AddPostRequestBody struct {
	Title       string `json:"title"`
	Content		string `json:"description"`
}

func (h handler) AddPost(c *gin.Context) {
    body := AddPostRequestBody{}

    // getting request's body
    if err := c.BindJSON(&body); err != nil {
        c.AbortWithError(http.StatusBadRequest, err)
        return
    }

    var post models.Post

    post.Title = body.Title
    post.Content = body.Content

    if result := h.DB.Create(&post); result.Error != nil {
        c.AbortWithError(http.StatusNotFound, result.Error)
        return
    }

    c.JSON(http.StatusCreated, &post)
}
