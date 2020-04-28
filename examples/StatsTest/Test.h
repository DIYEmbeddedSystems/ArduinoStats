#ifndef TEST_H
#define TEST_H

#define PRINTF(msg...)  Serial.printf(msg)

static uint32_t tst_count = 0;
static uint32_t tst_pass = 0;
static uint32_t tst_fail = 0;


#define tst_expect(cond, msg...) \
    do { \
      ++ tst_count; \
      if (cond) { \
        ++ tst_pass; \
        PRINTF("[PASS] %s:%d \t", __FILE__, __LINE__); \
        PRINTF(msg); \
      } else { \
        ++ tst_fail; \
        PRINTF("[FAIL] %s:%d. `%s` expected true, was false \t", __FILE__, __LINE__, #cond); \
        PRINTF(msg); \
      } \
      PRINTF("\n"); \
    } while(0) 


#define tst_expect_equal(a, b, type_fmt, msg...) \
    do { \
      ++ tst_count; \
      if ((a) == (b)) { \
        ++ tst_pass; \
        PRINTF("[PASS] %s:%d \t", __FILE__, __LINE__); \
      } else { \
        ++ tst_fail; \
        PRINTF("[FAIL] %s:%d expected `%s = %s`, evaluated " type_fmt " != " type_fmt "\t", \
            __FILE__, __LINE__, #a, #b, (a), (b)); \
        PRINTF(msg); \
      } \
      PRINTF("\n"); \
    } while(0)


#define tst_expect_in_range(x, low, high, type_fmt, msg...) \
    do { \
      ++ tst_count; \
      if ((low) <= (x) && (x) <= (high)) { \
        ++ tst_pass; \
      	PRINTF("[PASS] %s:%d \t", __FILE__, __LINE__); \
      } else { \
        ++ tst_fail; \
        PRINTF("[FAIL] %s:%d expected `%s` in range [%s ; %s], evaluated " type_fmt " not in [" type_fmt " ; " type_fmt "]\t", __FILE__, __LINE__, #x, #low, #high, (x), (low), (high)); \
        PRINTF(msg); \
      } \
      PRINTF("\n"); \
    } while(0)


#define tst_expect_equal_int(a, b, msg...) \
	tst_expect_equal(a, b, "%d", msg);


#define tst_expect_in_range_int(x, low, high, msg...) \
	tst_expect_in_range(x, low, high, "%d", msg);


#define tst_report() \
  do { \
    PRINTF("[TEST REPORT] Performed %d tests: %d passed (%2.2f%%), %d failed (%2.2f%%)\n", \
        tst_count, tst_pass, (100.*tst_pass)/tst_count, tst_fail, (100.*tst_fail)/tst_count); \
    tst_count = tst_pass = tst_fail = 0; \
  } while (0)

#endif
