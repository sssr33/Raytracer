#pragma once
class AlphaBlendStrategy{
public:
	virtual void AlphaBlend(void *destination, void *source, unsigned int numPixels) = 0;
protected:
	AlphaBlendStrategy(){}
};

class AlphaBlendSSE32BitStrategy : public AlphaBlendStrategy{
public:
	AlphaBlendSSE32BitStrategy();
	void AlphaBlend(void *destination, void *source, unsigned int numPixels);
private:
	void Blend4Pixels(unsigned int *destination, unsigned int *source);
	void Blend2Pixels(unsigned int *destination, unsigned int *source){}
	void Blend1Pixel(unsigned int *destination, unsigned int *source);

	//__declspec(align(16))unsigned int ADD255[4];// = {255, 255, 255, 255};
	//__declspec(align(16))unsigned int MASKALPHA[4];// = {  0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
	//__declspec(align(16))unsigned int MASKRED[4];// = {  0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000};
	//__declspec(align(16))unsigned int MASKGREEN[4];// = {0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00};
	//__declspec(align(16))unsigned int MASKBLUE[4];// = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};
};

class AlphaBlendSlow32BitStrategy : public AlphaBlendStrategy{
public:
	AlphaBlendSlow32BitStrategy(){}
	void AlphaBlend(void *destination, void *source, unsigned int numPixels) {}
private:
	void Blend4Pixels(unsigned int *destination, unsigned int *source);
	void Blend2Pixels(unsigned int *destination, unsigned int *source){}
	void Blend1Pixel(unsigned int *destination, unsigned int *source);
};
