#pragma once

#define GETBIT(__pBitArray, __bit)	(((__pBitArray)[(__bit) >> 3] & (1 << ((__bit) % 8))) > 0)
#define SETBIT(__pBitArray, __bit)	((__pBitArray)[((__bit) >> 3)] |= (1 << ((__bit) % 8)))

struct HuffNode
{
	UINT	ByteValue;
	UINT	Priority;
};

class CHuffman
{
public:
	CHuffman(void);
	~CHuffman(void);

private:
	UINT	m_CurrentBit;
	char	m_BitStream[32];
	//Queue (priority?)
	//Tree

	void	ClearBitCount();
	
	UINT	FindByte(BYTE* path, HuffNode& current);
	bool	FindPath(HuffNode& current, BYTE byte, BYTE* path);

	//May need to be a * instead of a &
	HuffNode&	BuildTree(HuffNode& Head);

	//May need to be a ** instead of a *&
	void		DeleteTree(HuffNode*& node);

	//Enqueue
	//Dequeue

};
