#include "FrameBufferObject.h"


void FrameBufferObject::InitFrameBuffer()
{
	CreateFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	CreateTextureAttachment();
	CreateDepthBufferAttachment();
}

void FrameBufferObject::CreateFrameBuffer()
{
	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	
}

void FrameBufferObject::CreateTextureAttachment()
{
	glGenTextures(1, &m_ColourTexture);
	glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FrameWidth, m_FrameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte *)NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexture, 0);
}

void FrameBufferObject::CreateDepthTextureAttachment()
{
	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_FrameWidth, m_FrameHeight, 0, GL_DEPTH_COMPONENT,
		GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthTexture, 0);
}

void FrameBufferObject::CreateDepthBufferAttachment()
{
	glGenRenderbuffers(1, &m_DepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_FrameWidth, m_FrameHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);

	
}

FrameBufferObject::FrameBufferObject(int p_Width, int p_Height, int p_ScreenWidth, int p_ScreenHeight)
{
	m_FrameWidth = p_Width;
	m_FrameHeight = p_Height;
	m_ScreenWidth = p_ScreenWidth;
	m_ScreenHeight = p_ScreenHeight;

	InitFrameBuffer();
	UnbindFrameBuffer();
}

void FrameBufferObject::CleanUp()
{
	glDeleteFramebuffers(1, &m_FrameBuffer);
	glDeleteTextures(1, &m_ColourTexture);
	glDeleteTextures(1, &m_DepthTexture);
	glDeleteTextures(1, &m_FrameBufferTextureID);

	glDeleteRenderbuffers(1, &m_DepthBuffer);
	glDeleteRenderbuffers(1, &m_ColourBuffer);

}

void FrameBufferObject::BindFrameBuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer);
	glViewport(0, 0, m_FrameWidth, m_FrameHeight);
}

void FrameBufferObject::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
}

void FrameBufferObject::BindToReadBuffer()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
}


int FrameBufferObject::GetColourTexture()
{
	return m_ColourTexture;
}

int FrameBufferObject::GetDepthTexture()
{
	return m_DepthTexture;
}

int FrameBufferObject::GetTextureID()
{
	return m_FrameBufferTextureID;
}
