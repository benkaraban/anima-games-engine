namespace Core
{
//-----------------------------------------------------------------------------
MTFUNCINLINE bool isValid(const float val)
{
	union { uint32 ui; float f; } convert;
    const uint32 expmask = (0xFF << 23);

    convert.f = val;
	
	uint32 iexp = convert.ui & expmask;
	bool invalid = (iexp == expmask);

	return !invalid;
}
//-----------------------------------------------------------------------------
MTFUNCINLINE bool isValid(const double val)
{
    union { uint64 ui; double d; } convert;
    const uint64 expmask = ((uint64)255 << 55);
    convert.d = val;

    bool invalid = ((convert.ui & expmask) == expmask);

    return !invalid;
}
//-----------------------------------------------------------------------------
}

