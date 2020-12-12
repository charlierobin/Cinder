/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/ip/Divide.h"

using namespace std;

namespace cinder { namespace ip {

template<bool DSTALPHA, bool DSTPREMULT, bool SRCPREMULT>
void divideImpl_u8( Surface8u *background, const Surface8u &foreground, const Area &srcArea, ivec2 absOffset )
{
	bool SRCALPHA = foreground.hasAlpha();
	const ptrdiff_t srcRowBytes = foreground.getRowBytes();
	const uint8_t sR = foreground.getChannelOrder().getRedOffset();
	const uint8_t sG = foreground.getChannelOrder().getGreenOffset();
	const uint8_t sB = foreground.getChannelOrder().getBlueOffset();
	const uint8_t sA = SRCALPHA ? (foreground.getChannelOrder().getAlphaOffset()) : 0;	
	const uint8_t srcInc = foreground.getPixelInc();
	const ptrdiff_t dstRowBytes = background->getRowBytes();
	const uint8_t dR = background->getChannelOrder().getRedOffset();
	const uint8_t dG = background->getChannelOrder().getGreenOffset();
	const uint8_t dB = background->getChannelOrder().getBlueOffset();
	const uint8_t dA = DSTALPHA ? (background->getChannelOrder().getAlphaOffset()) : 0;
	const uint8_t dstInc = background->getPixelInc();
	const int32_t width = srcArea.getWidth();
	
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		
		const uint8_t *src = reinterpret_cast<const uint8_t*>( reinterpret_cast<const uint8_t*>( foreground.getData() + srcArea.x1 * 4 ) + ( srcArea.y1 + y ) * srcRowBytes );
		
		uint8_t *dst = reinterpret_cast<uint8_t*>( reinterpret_cast<uint8_t*>( background->getData() + absOffset.x * 4 ) + ( y + absOffset.y ) * dstRowBytes );
		
		for( int32_t x = 0; x < width; ++x ) {
		
			dst[dR] = 255.0 * ( src[sR] / dst[dR] );
			dst[dG] = 255.0 * ( src[sG] / dst[dG] );
			dst[dB] = 255.0 * ( src[sB] / dst[dB] );
			
			src += srcInc;
			dst += dstInc;
		}
	}
}

void divide( Surface8u *background, const Surface8u &foreground, const Area &srcArea, const ivec2 &dstRelativeOffset )
{
	pair<Area,ivec2> srcDst = clippedSrcDst( foreground.getBounds(), srcArea, background->getBounds(), srcArea.getUL() + dstRelativeOffset );	
	if( background->hasAlpha() ) {
		if( background->isPremultiplied() ) {
			if( foreground.isPremultiplied() )
				divideImpl_u8<true, true, true>( background, foreground, srcDst.first, srcDst.second );
			else
				divideImpl_u8<true, true, false>( background, foreground, srcDst.first, srcDst.second );
		}
		else { // background unpremult
			if( foreground.isPremultiplied() )
				divideImpl_u8<true, false, true>( background, foreground, srcDst.first, srcDst.second );
			else
				divideImpl_u8<true, false, false>( background, foreground, srcDst.first, srcDst.second );
		}
	}
	else { // background no alpha
		if( foreground.isPremultiplied() )
			divideImpl_u8<false, false, true>( background, foreground, srcDst.first, srcDst.second );
		else
			divideImpl_u8<false, false, false>( background, foreground, srcDst.first, srcDst.second );
	}
}

void divide( Surface32f *background, const Surface32f &foreground, const Area &srcArea, const ivec2 &dstRelativeOffset )
{
	throw new Exception( "Not yet implemented" );
}

} } // namespace cinder::ip
