#include "Texture.h"

Texture::Texture(char* path) {
	loadTiff(path);
	filter = NEAREST;
}

Texture::Texture(int _w, int _h) {
	w = _w;
	h = _h;
	for (int i = 0; i < w*h; ++i) {
		pix[i] = 0x0;
	}
	filter = NEAREST;
}

bool Texture::isOpaque() {
	return (opacity == NULL);
}

float  Texture::clampCoordinate(float c, float off, int max) {
	int u = ((int)(c*(max - 1) + off)) % max;
	if (u < 0) u += max;
	return u;
}

void  Texture::setFilter(TexFilter flt) {
	filter = flt;
}

unsigned int Texture::getColor(float _u, float _v) {
	switch (filter) {
	case NEAREST:
		return getColorNearest(_u, _v);
	case BILINEAR:
		return getColorBilinear(_u, _v);
	}
}

unsigned int Texture::getColorNearest(float _u, float _v) {
	//clamp to 0,1 preserving offset
	int u = ((int)(_u*(w - 1))) % w;
	int v = ((int)((1 - _v)*(h - 1))) % h;
	if (u < 0) u += w;
	if (v < 0) v += h;
	//int ru = (int)u;
	//int rv = (int)v;
	//if (v * w + u > w * h || v * w + u < 0)
	//	cerr << ru << " , " << rv << " : " << u << " , " << v << " from: " << _u << "  ," << _v << "\n";

	return pix[v * w + u];
}

unsigned int Texture::getColorBilinear(float _u, float _v) {
	int left = clampCoordinate(_u, -0.5f, w);
	int right = clampCoordinate(_u, 0.5f, w);
	int bot = clampCoordinate(_v, -0.5f, h);
	int top = clampCoordinate(_v, 0.5f, h);

	float lweight = _u - left;
	float rweight = right - _u;
	float bweight = _v - bot;
	float tweight = top - _v;

	V3 a = V3(pix[top * w + left]);
	V3 b = V3(pix[top * w + right]);
	V3 c = V3(pix[bot * w + left]);
	V3 d = V3(pix[bot * w + right]);

	return ((a*lweight * tweight + b * rweight* tweight + c * lweight* bweight + d * rweight* bweight)).getColor();
}

float Texture::getOpacityNearest(float _u, float _v) {
	//clamp to 0,1 preserving offset
	int u = ((int)(_u*(w - 1))) % w;
	int v = ((int)((1 - _v)*(h - 1))) % h;
	if (u < 0) u += w;
	if (v < 0) v += h;
	int ru = (int)u;
	int rv = (int)v;
	//if (v * w + u > w * h || v * w + u < 0)
	//	cerr << ru << " , " << rv << " : " << u << " , " << v << " from: " << _u << "  ," << _v << "\n";
	return opacity[v * w + u];
}

void Texture::loadTiffTransparency(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (opacity == NULL) {
		delete[] opacity;
		opacity = new float[w*h];
		//clean up tmp
	}
	unsigned int* tmp = new unsigned int[w*h];

	if (TIFFReadRGBAImage(in, w, h, tmp, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	for (int i = 0; i < w*h; ++i) {
		V3 tmppixel(tmp[i]);
		opacity[i] = (tmppixel[0] + tmppixel[1] + tmppixel[2]) / 3;
	}

	TIFFClose(in);
	delete[] tmp;
}

// load a tiff image in grayscale to determine opacity.
void Texture::loadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w*h];
		//clean up tmp
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}