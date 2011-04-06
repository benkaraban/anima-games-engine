
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifndef LM_NOT_UNDEF_FAR
#ifdef far
#undef far
#endif
#endif/*LM_NOT_UNDEF_FAR*/

#ifndef LM_NOT_UNDEF_NEAR
#ifdef near
#undef near
#endif
#endif/*LM_NOT_UNDEF_NEAR*/
