package posts

import (
    "net/http"

    "github.com/gin-gonic/gin"
    "github.com/mgdawber/dawbs-api/pkg/common/models"
)

func (h handler) DeletePost(c *gin.Context) {
    id := c.Param("id")

    var post models.Post

    if result := h.DB.First(&post, id); result.Error != nil {
        c.AbortWithError(http.StatusNotFound, result.Error)
        return
    }

    h.DB.Delete(&post)

    c.Status(http.StatusOK)
}
