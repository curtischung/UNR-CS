/////////////////////////////////////////////////////////////////////////////
//
//	File: QzRect.h
//
//	$Header: /Projects/Qz/QzRect.h  8  2009/11/7 10:45:46a  Lee $
//
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


class QzRect
{
public:
	S32 m_Left;
	S32 m_Top;
	S32 m_Right;
	S32 m_Bottom;


	///////////////////////////////////////////////////////////////////////////
	//
	//	constructor
	//
	QzRect(void) {}
	QzRect(S32 l, S32 t, S32 r, S32 b) : m_Left(l), m_Top(t), m_Right(r), m_Bottom(b) {}


	///////////////////////////////////////////////////////////////////////////
	//
	//	+=()
	//
	QzRect& operator+=(const QzRect &src)
	{
	    m_Left   += src.m_Left;
		m_Top    += src.m_Top;
		m_Right  += src.m_Right;
		m_Bottom += src.m_Bottom;
		return *this;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	-=()
	//
	QzRect& operator-=(const QzRect &src)
	{
	    m_Left   -= src.m_Left;
		m_Top    -= src.m_Top;
		m_Right  -= src.m_Right;
		m_Bottom -= src.m_Bottom;
		return *this;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Move()
	//
	void Move(S32 dx, S32 dy)
	{
	    m_Left   += dx;
		m_Top    += dy;
		m_Right  += dx;
		m_Bottom += dy;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Set()
	//
	void Set(S32 l, S32 t)
	{
		m_Left   = l;
		m_Top    = t;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Set()
	//
	void Set(S32 l, S32 t, S32 r, S32 b)
	{
		m_Left   = l;
		m_Top    = t;
		m_Right  = r;
		m_Bottom = b;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Adjust()
	//
	void Adjust(S32 l, S32 t, S32 r, S32 b)
	{
		m_Left   += l;
		m_Top    += t;
		m_Right  += r;
		m_Bottom += b;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	SetSize()
	//
	void SetSize(S32 width, S32 height)
	{
		m_Right  = m_Left + width;
		m_Bottom = m_Top + height;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Width()
	//
	S32 Width(void) const
	{
		return m_Right - m_Left;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Height()
	//
	S32 Height(void) const
	{
		return m_Bottom - m_Top;
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	PointInRect()
	//
	bool PointInRect(const S32 x, const S32 y)
	{
		return ((x >= m_Left) && (x < m_Right) && (y >= m_Top) && (y < m_Bottom));
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Overlaps()
	//
	//	Returns true if the two rectangles overlap.  If they are discontinuous
	//	(from a render perspective), this will return false.
	//
	bool Overlaps(const QzRect &src) const
	{
		return ((m_Right >= src.m_Left) &&
				(m_Left < src.m_Right) &&
				(m_Bottom >= src.m_Top) &&
				(m_Top < src.m_Bottom));
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Contains()
	//
	//	Returns true if this rectangle completely contains the src rect.
	//
	bool Contains(const QzRect &src) const
	{
		return ((m_Left <= src.m_Left) &&
				(m_Right >= src.m_Right) &&
				(m_Top <= src.m_Top) &&
				(m_Bottom >= src.m_Bottom));
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	Crop()
	//
	//	Used to crop an existing bound to a new bound.  Assumes that this
	//	cropping rect was provided by the parent window, and needs to be
	//	cropped to the bounds of the child window.
	//
	void Crop(const QzRect &src)
	{
		if (m_Left < src.m_Left) {
			m_Left = src.m_Left;
		}

		if (m_Right > src.m_Right) {
			m_Right = src.m_Right;
		}

		if (m_Right < m_Left) {
			m_Right = m_Left;
		}

		if (m_Top < src.m_Top) {
			m_Top = src.m_Top;
		}

		if (m_Bottom > src.m_Bottom) {
			m_Bottom = src.m_Bottom;
		}

		if (m_Bottom < m_Top) {
			m_Bottom = m_Top;
		}
	}


	///////////////////////////////////////////////////////////////////////////
	//
	//	HasZeroSize()
	//
	bool HasZeroSize(void) const
	{
		return ((Width() <= 0) || (Height() <= 0));
	}
};


