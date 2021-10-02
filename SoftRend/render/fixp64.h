#pragma once
class fixp64 //fixed point 32.32
{
public:
	static __int64 double2fix(double d);
	static double fix2double(__int64 d);

	fixp64() {}
	fixp64(int i) : value((__int64)i << 32) {}
	fixp64(float f) : value(fixp64::double2fix(f)) {}
	fixp64(double d) : value(fixp64::double2fix(d)) {}

	fixp64& operator *= (fixp64 r) { value = mul(r); return *this; }
	fixp64& operator += (fixp64 r) { value += r.value; return *this; }
	fixp64& operator -= (fixp64 r) { value -= r.value; return *this; }

	fixp64& operator *= (int r) { value *= (__int64)r; return *this; }
	//fixp64& operator /= (int r) { intValue /= r; return *this; }

	fixp64 operator - () const { fixp64 x; x.value = -value; return x; }
	fixp64 operator + (fixp64 r) const { fixp64 x = *this; x += r; return x;}
	fixp64 operator - (fixp64 r) const { fixp64 x = *this; x -= r; return x;}
	fixp64 operator * (fixp64 r) const { fixp64 x = *this; x *= r; return x;}
	//fixp64 operator / (fixp64 r) const { fixp64 x = *this; x /= r; return x;}

	fixp64 operator + (int r) const { fixp64 x = *this; x += r; return x;}
	fixp64 operator - (int r) const { fixp64 x = *this; x -= r; return x;}
	fixp64 operator * (int r) const { fixp64 x = *this; x *= r; return x;}
	//fixp64 operator / (int r) const { fixp64 x = *this; x /= r; return x;}

	__int64 mul(fixp64 r)
	{
		__int64 ha, la;
		__int64 hb, lb;
		__int64 result, resulth, resultl, resulthalb, resulthbla;

		__int64 lp = 0x00000000FFFFFFFF;

		ha = this->value >> 32;
		hb = r.value >> 32;

		la = this->value & lp;
		lb = r.value & lp;

		resulth = (ha * hb) << 32;
		resultl = (la * lb) >> 32;
		resulthalb = ha * lb;
		resulthbla = hb * la;

		result = resultl + resulth + resulthalb + resulthbla;

		return result;
	}

	__int64 value;
};

