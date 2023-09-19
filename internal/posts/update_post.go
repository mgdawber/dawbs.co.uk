package posts

import (
    "net/http"

    "github.com/gin-gonic/gin"
    "github.com/mgdawber/dawbs-api/internal/common/models"
)

type UpdatePostRequestBody struct {
    Title       string `json:"title"`
    Content		string `json:"content"`
}

func (h handler) UpdatePost(c *gin.Context) {
    id := c.Param("id")
    body := UpdatePostRequestBody{}

    // getting request's body
    if err := c.BindJSON(&body); err != nil {
        c.AbortWithError(http.StatusBadRequest, err)
        return
    }

    var post models.Post

    if result := h.DB.First(&post, id); result.Error != nil {
        c.AbortWithError(http.StatusNotFound, result.Error)
        return
    }

    post.Title = body.Title
    post.Content = body.Content

    h.DB.Save(&post)

    c.JSON(http.StatusOK, &post)
}
