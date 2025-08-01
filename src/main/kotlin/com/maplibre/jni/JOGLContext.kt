package com.maplibre.jni

import com.jogamp.opengl.GLContext as JOGLGLContext
import com.jogamp.opengl.GLDrawable
import com.jogamp.common.nio.Buffers
import com.jogamp.opengl.GL
import java.nio.ByteBuffer

/**
 * JOGL implementation of GLContext interface.
 */
class JOGLContext(
    private val glContext: JOGLGLContext,
    private val drawable: GLDrawable
) : GLContext, AutoCloseable {
    
    override fun makeCurrent() {
        val result = glContext.makeCurrent()
        if (result == JOGLGLContext.CONTEXT_NOT_CURRENT) {
            throw RuntimeException("Failed to make GL context current")
        }
    }
    
    override fun release() {
        glContext.release()
    }
    
    override fun getProcAddress(name: String): Long {
        // JOGL handles function pointer resolution internally
        // For now, we need to use a native method to access platform-specific getProcAddress
        // This will be implemented as a JNI function that calls the appropriate
        // platform function (wglGetProcAddress on Windows, glXGetProcAddress on Linux, etc.)
        return nativeGetProcAddress(name)
    }
    
    override fun close() {
        // Release the context if it's current
        if (glContext.isCurrent) {
            glContext.release()
        }
    }
    
    companion object {
        @JvmStatic
        private external fun nativeGetProcAddress(name: String): Long
    }
}