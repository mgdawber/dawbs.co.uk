package posts

import (
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/mgdawber/dawbs-api/internal/common/models"
)

func (h handler) GetPosts(c *gin.Context) {
	var posts []models.Post

	if result := h.DB.Find(&posts); result.Error != nil {
		c.AbortWithError(http.StatusNotFound, result.Error)
		return
	}

	c.JSON(http.StatusOK, &posts)
}
