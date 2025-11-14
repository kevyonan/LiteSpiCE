#ifndef REALTYPE_H_INCLUDED
#	define REALTYPE_H_INCLUDED

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#define RATIONAL_EXPORT    static inline

#	ifdef TICE_H
#include <ti/real.h>
typedef real_t rat;
#define PRIRAT    "f"

/** Unary Operations */
RATIONAL_EXPORT float rat_to_float(rat const a) {
	return os_RealToFloat(&a);
}
RATIONAL_EXPORT rat float_to_rat(float const a) {
	return os_FloatToReal(a);
}

RATIONAL_EXPORT rat rat_pos1(void) {
	return os_Int24ToReal(1);
}
RATIONAL_EXPORT rat rat_neg1(void) {
	return os_Int24ToReal(-1);
}
RATIONAL_EXPORT rat rat_zero(void) {
	return os_Int24ToReal(0);
}
RATIONAL_EXPORT rat rat_from_int(int24_t const a) {
	return os_Int24ToReal(a);
}
RATIONAL_EXPORT rat rat_frac(rat const a) {
	return os_RealFrac(&a);
}
RATIONAL_EXPORT rat rat_int(rat const a) {
	return os_RealInt(&a);
}
RATIONAL_EXPORT rat rat_neg(rat const a) {
	return os_RealNeg(&a);
}
RATIONAL_EXPORT rat rat_ln(rat const a) {
	return os_RealLog(&a);
}
RATIONAL_EXPORT rat rat_exp(rat const a) {
	return os_RealExp(&a);
}
RATIONAL_EXPORT rat rat_recip(rat const a) {
	return os_RealInv(&a);
}
RATIONAL_EXPORT rat rat_floor(rat const a) {
	return os_RealFloor(&a);
}
RATIONAL_EXPORT rat rat_rad_to_deg(rat const a) {
	return os_RealRadToDeg(&a);
}
RATIONAL_EXPORT rat rat_deg_to_rad(rat const a) {
	return os_RealDegToRad(&a);
}
RATIONAL_EXPORT rat rat_sin(rat const a) {
	return os_RealSinRad(&a);
}
RATIONAL_EXPORT rat rat_cos(rat const a) {
	return os_RealCosRad(&a);
}
RATIONAL_EXPORT rat rat_tan(rat const a) {
	return os_RealTanRad(&a);
}
RATIONAL_EXPORT rat rat_asin(rat const a) {
	return os_RealAsinRad(&a);
}
RATIONAL_EXPORT rat rat_acos(rat const a) {
	return os_RealAcosRad(&a);
}
RATIONAL_EXPORT rat rat_atan(rat const a) {
	return os_RealAtanRad(&a);
}
RATIONAL_EXPORT rat rat_sqrt(rat const a) {
	return os_RealSqrt(&a);
}
RATIONAL_EXPORT rat rat_pi(void) {
	return rat_acos(rat_neg1());
}

/** Binary Operations */
RATIONAL_EXPORT rat rat_add(rat const a, rat const b) {
	return os_RealAdd(&a, &b);    /// a + b
}
RATIONAL_EXPORT rat rat_sub(rat const a, rat const b) {
	return os_RealSub(&a, &b);    /// a - b
}
RATIONAL_EXPORT rat rat_mul(rat const a, rat const b) {
	return os_RealMul(&a, &b);    /// a * b
}
RATIONAL_EXPORT rat rat_addmul(rat const a, rat const b, rat const c) {
	return rat_add(a, rat_mul(b, c));    /// a + b*c
}
RATIONAL_EXPORT rat rat_div(rat const a, rat const b) {
	return os_RealDiv(&a, &b);    /// a / b
}
RATIONAL_EXPORT rat rat_mod(rat const a, rat const b) {
	return os_RealMod(&a, &b);    /// a % b
}
RATIONAL_EXPORT rat rat_pow(rat const a, rat const b) {
	return os_RealPow(&a, &b);    /// a^b
}
RATIONAL_EXPORT rat rat_min(rat const a, rat const b) {
	return os_RealMin(&a, &b);    /// a < b? a : b;
}
RATIONAL_EXPORT rat rat_max(rat const a, rat const b) {
	return os_RealMax(&a, &b);    /// a < b? b : a;
}
RATIONAL_EXPORT int rat_cmp(rat const a, rat const b) {
	return os_RealCompare(&a, &b);
}
RATIONAL_EXPORT rat rat_log_base(rat const a, rat const b) {
	return rat_div(rat_ln(a), rat_ln(b));
}
RATIONAL_EXPORT int rat_lt(rat const a, rat const b) {
	return rat_cmp(a, b) < 0;
}
RATIONAL_EXPORT rat rat_abs(rat const a) {
	return rat_lt(a, rat_zero())? rat_neg(a) : a;
}
RATIONAL_EXPORT int rat_ge(rat const a, rat const b) {
	return rat_cmp(a, b) >= 0;
}
RATIONAL_EXPORT int rat_le(rat const a, rat const b) {
	return rat_cmp(a, b) <= 0;
}

RATIONAL_EXPORT rat rat_epsilon(void) {
	rat eps = rat_pos1();
	rat const one = rat_pos1();
	rat const two = rat_from_int(2);
	while( rat_lt(one, rat_add(eps, one)) ) {
		eps = rat_div(eps, two);
	}
	return rat_mul(eps, two);
}

/** Ternary Operations */
RATIONAL_EXPORT rat rat_clamp(rat const val, rat const min, rat const max) {
	return rat_max(min, rat_min(val, max));
}
RATIONAL_EXPORT int rat_eq(rat const a, rat const b, rat const eps) {
	return rat_lt(rat_abs(rat_sub(a, b)), eps);
}

RATIONAL_EXPORT rat rat_root(rat const a, rat const b) {
	rat const eps = rat_epsilon();
	if( rat_eq(b, rat_from_int(2), eps) ) {
		return rat_sqrt(a);
	}
	rat const r = os_RealInv(&b);
	return os_RealPow(&a, &r);    /// a^(1/b)
}

RATIONAL_EXPORT int rat_to_str(rat const a, size_t const len, char buffer[const static len]) {
	return os_RealToStr(buffer, &a, len, 0, -1) > 0;
}

RATIONAL_EXPORT char *rat_to_cstr(rat const a, size_t const len, char buffer[const static len]) {
	os_RealToStr(buffer, &a, len, 0, -1);
	return buffer;
}

RATIONAL_EXPORT rat str_to_rat(char const cstr[const static 1]) {
	char *end = NULL;
	return os_StrToReal(cstr, &end);
}

RATIONAL_EXPORT rat rat_sinh(rat const a) {
	rat const epx  = rat_exp(a);
	rat const enx  = rat_recip(epx);
	rat const half = rat_recip(rat_from_int(2));
	return rat_mul(rat_sub(epx, enx), half);
}

RATIONAL_EXPORT rat rat_cosh(rat const a) {
	rat const epx  = rat_exp(a);
	rat const enx  = rat_recip(epx);
	rat const half = rat_recip(rat_from_int(2));
	return rat_mul(rat_add(epx, enx), half);
}

RATIONAL_EXPORT rat rat_tanh(rat const a) {
	rat const epx = rat_exp(a);
	rat const enx = rat_recip(epx);
	return rat_div(rat_sub(epx, enx), rat_add(epx, enx));
}

RATIONAL_EXPORT rat rat_round(rat const a, unsigned const digits) {
	if( digits==0 ) {
		return os_RealRoundInt(&a);
	}
	return os_RealRound(&a, ( char )(digits));
}

#	else
#include <tgmath.h>
typedef long double    rat;
#define PRIRAT    "Lf"

/** Unary Operations */
RATIONAL_EXPORT float rat_to_float(rat const a) {
	return ( float )(a);
}
RATIONAL_EXPORT rat float_to_rat(float const a) {
	return ( rat )(a);
}

RATIONAL_EXPORT rat rat_pos1(void) {
	return 1.;
}
RATIONAL_EXPORT rat rat_neg1(void) {
	return -1.;
}
RATIONAL_EXPORT rat rat_zero(void) {
	return 0;
}
RATIONAL_EXPORT rat rat_from_int(int const a) {
	return a + 0.0;
}
RATIONAL_EXPORT rat rat_frac(rat const a) {
	return a - floor(a);
}
RATIONAL_EXPORT rat rat_int(rat const a) {
	return floor(a);
}
RATIONAL_EXPORT rat rat_neg(rat const a) {
	return -a;
}
RATIONAL_EXPORT rat rat_abs(rat const a) {
	return fabs(a);
}
RATIONAL_EXPORT rat rat_ln(rat const a) {
	return log(a);
}
RATIONAL_EXPORT rat rat_exp(rat const a) {
	return exp(a);
}
RATIONAL_EXPORT rat rat_recip(rat const a) {
	return 1.0 / a;
}
RATIONAL_EXPORT rat rat_floor(rat const a) {
	return floor(a);
}
RATIONAL_EXPORT rat rat_rad_to_deg(rat const a) {
	return a * (180.0 / acos(-1.0));
}
RATIONAL_EXPORT rat rat_deg_to_rad(rat const a) {
	return a * (acos(-1.0) / 180.0);
}
RATIONAL_EXPORT rat rat_sin(rat const a) {
	return sin(a);
}
RATIONAL_EXPORT rat rat_cos(rat const a) {
	return cos(a);
}
RATIONAL_EXPORT rat rat_tan(rat const a) {
	return tan(a);
}
RATIONAL_EXPORT rat rat_asin(rat const a) {
	return asin(a);
}
RATIONAL_EXPORT rat rat_acos(rat const a) {
	return acos(a);
}
RATIONAL_EXPORT rat rat_atan(rat const a) {
	return atan(a);
}
RATIONAL_EXPORT rat rat_sqrt(rat const a) {
	return sqrt(a);
}
RATIONAL_EXPORT rat rat_pi(void) {
	return acos(-1.0);
}
RATIONAL_EXPORT rat rat_epsilon(void) {
	rat eps = 1.0;
	while( eps+1.0 > 1.0 ) {
		eps *= 0.5;
	}
	return eps * 2.0;
}

/** Binary Operations */
RATIONAL_EXPORT rat rat_add(rat const a, rat const b) {
	return a + b;
}
RATIONAL_EXPORT rat rat_sub(rat const a, rat const b) {
	return a - b;
}
RATIONAL_EXPORT rat rat_mul(rat const a, rat const b) {
	return a * b;
}
RATIONAL_EXPORT rat rat_addmul(rat const a, rat const b, rat const c) {
	return rat_add(a, rat_mul(b, c));    /// a + b*c
}
RATIONAL_EXPORT rat rat_div(rat const a, rat const b) {
	return a / b;
}
RATIONAL_EXPORT rat rat_mod(rat const a, rat const b) {
	return fmod(a, b);
}
RATIONAL_EXPORT rat rat_pow(rat const a, rat const b) {
	return pow(a,b);
}
RATIONAL_EXPORT rat rat_min(rat const a, rat const b) {
	return a < b? a : b;
}
RATIONAL_EXPORT rat rat_max(rat const a, rat const b) {
	return a < b? b : a;
}
RATIONAL_EXPORT int rat_cmp(rat const a, rat const b) {
	rat const c = a - b;
	return c<0? -1 : c>0? 1 : 0;
}
RATIONAL_EXPORT rat rat_log_base(rat const a, rat const b) {
	return log(a) / log(b);
}
RATIONAL_EXPORT int rat_lt(rat const a, rat const b) {
	return a < b;
}
RATIONAL_EXPORT int rat_ge(rat const a, rat const b) {
	return a >= b;
}
RATIONAL_EXPORT int rat_le(rat const a, rat const b) {
	return a <= b;
}

/** Ternary Operations */
RATIONAL_EXPORT rat rat_clamp(rat const val, rat const min, rat const max) {
	return rat_max(min, rat_min(val, max));
}
RATIONAL_EXPORT int rat_eq(rat const a, rat const b, rat const eps) {
	return fabs(a-b) < eps;
}

RATIONAL_EXPORT rat rat_root(rat const a, rat const b) {
	rat const eps = rat_epsilon();
	if( rat_eq(b, 2.0, eps) ) {
		return sqrt(a);
	} else if( rat_eq(b, 3.0, eps) ) {
		return cbrt(a);
	}
	return pow(a, 1/b);
}

RATIONAL_EXPORT int rat_to_str(rat const a, size_t const len, char buffer[const static len]) {
	return snprintf(buffer, len, "%" PRIRAT "", a);
}

RATIONAL_EXPORT char *rat_to_cstr(rat const a, size_t const len, char buffer[const static len]) {
	( void )(rat_to_str(a, len, buffer));
	return buffer;
}

RATIONAL_EXPORT rat str_to_rat(char const cstr[const static 1]) {
	return strtold(cstr, NULL);
}

RATIONAL_EXPORT rat rat_sinh(rat const a) {
	return sinh(a);
}

RATIONAL_EXPORT rat rat_cosh(rat const a) {
	return cosh(a);
}

RATIONAL_EXPORT rat rat_tanh(rat const a) {
	return tanh(a);
}

RATIONAL_EXPORT rat rat_round(rat const a, unsigned const digits) {
	if( digits==0 ) {
		return round(a);
	}
	rat const tenth_pow = pow(10.0L, digits+0.L);
	return round(a * tenth_pow) / tenth_pow;
}
#	endif


/// complex/imaginary type.
typedef struct {
	rat real,imag;
} cplx;

RATIONAL_EXPORT rat cplx_real(cplx const a) { return a.real; }
RATIONAL_EXPORT rat cplx_imag(cplx const a) { return a.imag; }

RATIONAL_EXPORT cplx cplx_make(rat const real, rat const imag) {
	return ( cplx ){ real, imag };
}
RATIONAL_EXPORT cplx cplx_from_polar(rat const mag, rat const phase) {
	return ( cplx ){ rat_mul(mag, rat_cos(phase)), rat_mul(mag, rat_sin(phase)) };
}

RATIONAL_EXPORT cplx cplx_zero(void) {
	return ( cplx ){ rat_zero(),rat_zero() };
}
RATIONAL_EXPORT cplx cplx_one(void) {
	return ( cplx ){ rat_pos1(), rat_zero() };
}
RATIONAL_EXPORT cplx cplx_i(void) {
	return ( cplx ){ rat_zero(), rat_pos1() };
}


RATIONAL_EXPORT cplx cplx_add_rat(cplx a, rat const b) {
	a.real = rat_add(a.real, b);
	return a;
}
RATIONAL_EXPORT cplx cplx_sub_rat(cplx a, rat const b) {
	a.real = rat_sub(a.real, b);
	return a;
}
RATIONAL_EXPORT cplx cplx_mul_rat(cplx const a, rat const b) {
	return ( cplx ){ rat_mul(a.real, b), rat_mul(a.imag, b) };
}
RATIONAL_EXPORT cplx cplx_div_rat(cplx const a, rat const b) {
	return ( cplx ){ rat_div(a.real, b), rat_div(a.imag, b) };
}

RATIONAL_EXPORT cplx cplx_add_cplx(cplx const a, cplx const b) {
	return ( cplx ){ rat_add(a.real, b.real), rat_add(a.imag, b.imag) };
}
RATIONAL_EXPORT cplx cplx_sub_cplx(cplx const a, cplx const b) {
	return ( cplx ){ rat_sub(a.real, b.real), rat_sub(a.imag, b.imag) };
}
RATIONAL_EXPORT cplx cplx_mul_cplx(cplx a, cplx const b) {
	rat const ac = rat_mul(a.real, b.real);
	rat const bd = rat_mul(a.imag, b.imag);
	rat const ad = rat_mul(a.real, b.imag);
	rat const bc = rat_mul(a.imag, b.real);
	return ( cplx ){ rat_sub(ac, bd), rat_add(ad, bc) };
}
RATIONAL_EXPORT cplx cplx_div_cplx(cplx a, cplx const b) {
	/// denom = c^2 + d^2
	rat const denom = rat_add(rat_mul(b.real, b.real), rat_mul(b.imag, b.imag));
	
	/// (a+bi)(c-di) = (ac+bd) + (bc-ad)i
	rat const ac = rat_mul(a.real, b.real);
	rat const bd = rat_mul(a.imag, b.imag);
	rat const bc = rat_mul(a.imag, b.real);
	rat const ad = rat_mul(a.real, b.imag);
	return ( cplx ){ rat_div(rat_add(ac, bd), denom), rat_div(rat_sub(bc, ad), denom) };
}

RATIONAL_EXPORT cplx cplx_neg(cplx const a) {
	return ( cplx ){ rat_neg(a.real), rat_neg(a.imag) };
}

RATIONAL_EXPORT cplx cplx_conj(cplx const a) {
	return ( cplx ){ a.real, rat_neg(a.imag) };
}

RATIONAL_EXPORT rat cplx_abs(cplx const a) {
	rat const sum = rat_add(rat_mul(a.real, a.real), rat_mul(a.imag, a.imag));
	return rat_sqrt(sum);
}

RATIONAL_EXPORT rat cplx_phase(cplx const a) {
	rat const x    = a.real;
	rat const y    = a.imag;
	rat const zero = rat_zero();
	rat const eps  = rat_epsilon();
	if( rat_eq(x, zero, eps) ) {
		if( rat_lt(zero, y) ) {
			return rat_div(rat_pi(), rat_from_int(2));
		} else if( rat_lt(y, zero) ) {
			rat const half_pi = rat_div(rat_pi(), rat_from_int(2));
			return rat_neg(half_pi);
		}
		return zero;
	}
	
	rat const t = rat_atan(rat_div(y, x));
	if( rat_lt(zero, x) ) {
		return t;
	} else if( rat_ge(y, zero) ) {
		return rat_add(t, rat_pi());
	}
	return rat_sub(t, rat_pi());
}

RATIONAL_EXPORT cplx cplx_norm(cplx const a) {
	rat const mag = cplx_abs(a);
	rat const zero = rat_zero();
	if( rat_eq(mag, zero, rat_epsilon()) ) {
		return ( cplx ){ zero, zero };
	}
	return cplx_div_rat(a, mag);
}

RATIONAL_EXPORT int cplx_eq(cplx const a, cplx const b, rat const eps) {
	cplx const diff = cplx_sub_cplx(a, b);
	return rat_lt(cplx_abs(diff), eps);
}

RATIONAL_EXPORT cplx cplx_sqrt(cplx const a) {
	rat const zero = rat_zero();
	rat const eps  = rat_epsilon();
	if( rat_eq(a.real, zero, eps) && rat_eq(a.imag, zero, eps) ) {
		return ( cplx ){ zero, zero };
	}
	
	rat const r     = cplx_abs(a);
	rat const half  = rat_recip(rat_from_int(2));
	rat const u_arg = rat_mul(rat_add(r, a.real), half);
	rat const u     = rat_sqrt(u_arg);
	rat const v_arg = rat_mul(rat_sub(r, a.real), half);
	rat       v     = rat_sqrt(v_arg);
	if( rat_lt(a.imag, zero) ) {
		v = rat_neg(v);
	}
	return ( cplx ){ u, v };
}

RATIONAL_EXPORT cplx cplx_exp(cplx const a) {
	rat const ex = rat_exp(a.real);
	return ( cplx ){ rat_mul(ex, rat_cos(a.imag)), rat_mul(ex, rat_sin(a.imag)) };
}

RATIONAL_EXPORT cplx cplx_ln(cplx const a) {
	return ( cplx ){ rat_ln(cplx_abs(a)), cplx_phase(a) };
}

RATIONAL_EXPORT cplx cplx_pow_real(cplx const a, rat const p) {
	rat const r_pow   = rat_pow(cplx_abs(a), p);
	rat const p_theta = rat_mul(p, cplx_phase(a));
	return ( cplx ){ rat_mul(r_pow, rat_cos(p_theta)), rat_mul(r_pow, rat_sin(p_theta)) };
}

RATIONAL_EXPORT cplx cplx_pow_cplx(cplx const a, cplx const p) {
	cplx const x = cplx_ln(a);
	return cplx_exp(cplx_mul_cplx(p, x));
}

RATIONAL_EXPORT cplx cplx_sin(cplx const a) {
	rat const x = a.real;
	rat const y = a.imag;
	return ( cplx ){ rat_mul(rat_sin(x), rat_cosh(y)), rat_mul(rat_cos(x), rat_sinh(y)) };
}

RATIONAL_EXPORT cplx cplx_cos(cplx const a) {
	rat const x = a.real;
	rat const y = a.imag;
	return ( cplx ){ rat_mul(rat_cos(x), rat_cosh(y)), rat_neg(rat_mul(rat_sin(x), rat_sinh(y))) };
}

RATIONAL_EXPORT cplx cplx_tan(cplx const a) {
	return cplx_div_cplx(cplx_sin(a), cplx_cos(a));
}

RATIONAL_EXPORT rat cplx_to_polar(cplx const a, rat *const phase_ref) {
	if( phase_ref != NULL ) {
		*phase_ref = cplx_phase(a);
	}
	return cplx_abs(a);
}

RATIONAL_EXPORT cplx cplx_round(cplx const a, unsigned const digits) {
	return ( cplx ){ rat_round(a.real,digits), rat_round(a.imag,digits) };
}

RATIONAL_EXPORT cplx str_to_cplx(char const cstr[const static 1]) {
	rat const zero = rat_zero();
	if( cstr==NULL || cstr[0]==0 ) {
		return cplx_zero();
	}
	
	char const *j_ptr = strrchr(cstr, 'j');
	if( j_ptr==NULL ) {
		return ( cplx ){ str_to_rat(cstr), zero };
	}
	
	size_t const pre_len = ( size_t )(j_ptr - cstr);
	char real_buf[64]={0};
	char imag_buf[64]={0};
	if( pre_len==0 ) {
		return ( cplx ){ zero, rat_pos1() };
	}
	
	size_t split = 0;
	for( size_t i=1; i < pre_len; i++ ) {
		char const ch = cstr[i];
		if( ch=='+' || ch=='-' ) {
			split = i;
		}
	}
	
	if( split==0 ) {
		size_t imag_len = pre_len;
		if( imag_len >= sizeof imag_buf ) {
			imag_len = sizeof imag_buf - 1;
		}
		memcpy(imag_buf, cstr, imag_len);
		imag_buf[imag_len] = 0;
		if( (imag_buf[0]=='+' || imag_buf[0]=='-') && imag_buf[1]==0 ) {
			imag_buf[1] = '1';
			imag_buf[2] = 0;
		}
		return ( cplx ){ zero, str_to_rat(imag_buf) };
	}
	
	size_t real_len = split;
	if( real_len >= sizeof real_buf ) {
		real_len = sizeof real_buf - 1;
	}
	memcpy(real_buf, cstr, real_len);
	real_buf[real_len] = 0;
	
	size_t imag_len = pre_len - split;
	if( imag_len >= sizeof imag_buf ) {
		imag_len = sizeof imag_buf - 1;
	}
	
	memcpy(imag_buf, cstr + split, imag_len);
	imag_buf[imag_len] = 0;
	if( (imag_buf[0]=='+' || imag_buf[0]=='-') && imag_buf[1]==0 ) {
		imag_buf[1] = '1';
		imag_buf[2] = 0;
	}
	return ( cplx ){ str_to_rat(real_buf), str_to_rat(imag_buf) };
}

RATIONAL_EXPORT int cplx_to_str(cplx const a, size_t const len, char buffer[const static len]) {
	if( len==0 ) {
		return 0;
	}
#ifdef TICE_H
	char real_buf[32] = {0};
	char imag_buf[32] = {0};
	rat_to_cstr(a.real, sizeof real_buf, real_buf);
	rat_to_cstr(a.imag, sizeof imag_buf, imag_buf);
	char sign = '+';
	char const *imag_digits = imag_buf;
	if( imag_buf[0]=='-' ) {
		sign = '-';
		imag_digits = imag_buf + 1;
	}
	return snprintf(buffer, len, "%s%c%si", real_buf, sign, imag_digits);
#else
	return snprintf(buffer, len, "%Lf%+Lfi", a.real, a.imag);
#endif
}

RATIONAL_EXPORT char *cplx_to_cstr(cplx const a, size_t const len, char buffer[const static len]) {
	( void )(cplx_to_str(a, len, buffer));
	return buffer;
}
#endif