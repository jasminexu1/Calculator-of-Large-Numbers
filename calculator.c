#include <stdlib.h>
#include "biggie.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

struct biggie {
  bool negative;
  char *digits;
};

struct biggie *biggie_create(const char *s) {
  assert(s);
  int len = strlen(s);
  int index = 0;
  struct biggie *big = malloc(sizeof(struct biggie));
  if (s[0] == '-') {
    index++;
    big->negative = true;
  } else {
    big->negative = false;
  }
  if (s[index] == '0') {
    if (big->negative == 1) {
      free(big);
      return NULL;
    } else if (index == 0 && s[1] != '\0') {
      free(big);
      return NULL;
    } else if (index == 1) {
      free(big);
      return NULL;
    }
  }
  int digit_len = len - index + 1;
  big->digits = malloc(digit_len * sizeof(char));
  for(int i = 0; i < digit_len - 1; ++i) {
    if (s[len - i - 1] >= '0' && s[len - i - 1] <= '9') {
      big->digits[i] = s[len - i - 1];
    } else {
      free(big->digits);
      free(big);
      return NULL;
    }
  }
  big->digits[digit_len - 1] = '\0';
  return big;
};

// see biggie.h for documentation
void biggie_destroy(struct biggie *big) {
  assert(big);
  free(big->digits);
  free(big);
}

// see biggie.h for documentation
void biggie_print(const struct biggie *big, bool newline) {
  assert(big);
  if (big->negative == 1) {
    printf("-");
  }
  int len = strlen(big->digits);
  for (int i = 0; i < len; ++i) {
    printf("%c", big->digits[len - i - 1]);
  }
  if (newline == 1) {
    printf("\n");
  }
}

static int compare(const char *num1, const char *num2) {
  assert(num1);
  assert(num2);
  int len1 = strlen(num1);
  int len2 = strlen(num2);
  if (len1 > len2) {
    return 1;
  } else if (len1 < len2) {
    return -1;
  }
  for (int i = len1 - 1; i >= 0; --i) {
    if (num1[i] > num2[i]) {
      return 1;
    } else if (num1[i] < num2[i]) {
      return -1;
    }
  }
  return 0;
}

static void add_calc(struct biggie *n, const char *num1, const char *num2) {
  assert(n);
  assert(num1);
  assert(num2);
  int len1 = strlen(num1);
  int len2 = strlen(num2);
  int len = 0;
  if (len1 >= len2) {
    len = len1;
  } else {
    len = len2;
  }
  int *save = malloc(len * sizeof(int));
  for (int i = 0; i < len; ++i) {
    save[i] = 0;
  }
  int save_len = len;
  int sum = 0;
  for (int i = 0; i < len; ++i) {
    if (i >= len1) {
      sum = num2[i] - '0';
    } else if (i >= len2) {
      sum = num1[i] - '0';
    } else {
      sum = num1[i] - '0' + num2[i] - '0';
    }
    sum += save[i];
    if (sum >= 10) {
      if (i + 1 >= save_len) {
        save = realloc(save, (save_len + 1) * sizeof(int));
        save[i + 1] = 0;
        save_len++;
      }
      save[i + 1] += sum / 10;
    }
    save[i] = sum % 10;
  }
  int count_zero = 0;
  for (int t = save_len - 1; t >= 0; --t) {
    if (save[t] != 0) {
      break;
    }
    count_zero++;
  }
  n->digits = realloc(n->digits, (save_len - count_zero + 1) * sizeof(char));
  for (int x = 0; x < save_len - count_zero; ++x) {
    n->digits[x] = save[x] + '0';
  }
  n->digits[save_len - count_zero] = '\0';
  free(save);
}

static void sub_calc(struct biggie *n, const char *num1, const char *num2) {
  assert(n);
  assert(num1);
  assert(num2);
  assert(compare(num1, num2) > 0);
  int len1 = strlen(num1);
  int len2 = strlen(num2);
  int *save = malloc(len1 * sizeof(int));
  for (int j = 0; j < len1; ++j) {
    save[j] = num1[j];
  }
  for (int i = 0; i < len1; ++i) {
    if (i >= len2) {
      break;
    }
    if (save[i] >= num2[i]) {
      save[i] = save[i] - num2[i] + '0';
    } else {
      int k = i + 1;
      if (save[k] == '0') {
        while (1) {
          if (save[k] == '0') {
            save[k] = '9';
            k++;
          } else {
            break;
          }
        }
      }
      save[k] -= 1;
      save[i] = 10 + (save[i] - '0') - (num2[i] - '0') + '0';
    }
  }
  int count_zero = 0;
  for (int t = len1 - 1; t >= 0; --t) {
    if (save[t] != '0') {
      break;
    }
    count_zero++;
  }
  n->digits = realloc(n->digits, (len1 - count_zero + 1) * sizeof(char));
  for (int x = 0; x < len1 - count_zero; ++x) {
    n->digits[x] = save[x];
  }
  n->digits[len1 - count_zero] = '\0';
  free(save);
}

void biggie_add(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int comp = compare(n->digits, m->digits);
  if (n->digits[0] == '0' && n->digits[1] == '\0' && m->digits[0] == '0' &&
      m->digits[1] == '\0' && n->negative == 0 && m->negative == 0) {
      return;
  } else if (n->negative == 1) {
    if (m->negative == 1) {
      add_calc(n, n->digits, m->digits);
    } else if (comp > 0) {
      sub_calc(n, n->digits, m->digits);
    } else if (comp < 0) {
      n->negative = false;
      sub_calc(n, m->digits, n->digits);
    } else if (comp == 0) {
      n->digits = realloc(n->digits, 2 * sizeof(char));
      n->negative = false;
      n->digits[0] = '0';
    }
  } else if (n->negative == 0) {
    if (m->negative == 0) {
      add_calc(n, n->digits, m->digits);
    } else if (comp > 0) {
      sub_calc(n, n->digits, m->digits);
    } else if (comp < 0) {
      n->negative = true;
      sub_calc(n, m->digits, n->digits);
    } else if (comp == 0) {
      n->digits = realloc(n->digits, 2 * sizeof(char));
      n->negative = false;
      n->digits[0] = '0';
    }
  }
}

void biggie_sub(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int comp = compare(n->digits, m->digits);
  if (n->negative == 1) {
    if (m->negative == 0) {
      add_calc(n, n->digits, m->digits);
    } else if (comp > 0) {
      sub_calc(n, n->digits, m->digits);
    } else if (comp < 0) {
      n->negative = false;
      sub_calc(n, m->digits, n->digits);
    } else if (comp == 0) {
      n->digits = realloc(n->digits, 2 * sizeof(char));
      n->negative = false;
      n->digits[0] = '0';
    }
  } else if (n->negative == 0) {
    if (m->negative == 1) {
      add_calc(n, n->digits, m->digits);
    } else if (comp > 0) {
      sub_calc(n, n->digits, m->digits);
    } else if (comp < 0) {
      n->negative = true;
      sub_calc(n, m->digits, n->digits);
    } else if (comp == 0) {
      n->digits = realloc(n->digits, 2 * sizeof(char));
      n->negative = false;
      n->digits[0] = '0';
    }
  }
}

static void mult_calc(struct biggie *n, char *num1, const char *num2) {
  assert(n);
  assert(num1);
  assert(num2);
  int len1 = strlen(num1);
  int len2 = strlen(num2);
  int len = len1 + len2;
  int *save_int = malloc(len * sizeof(int));
  for (int s = 0; s < len; ++s) {
    save_int[s] = 0;
  }
  for (int i = 0; i < len1; ++i) {
    for (int j = 0; j < len2; ++j) {
      save_int[i + j] += (num1[i] - '0') * (num2[j] - '0');
      if (save_int[i + j] >= 10) {
        save_int[i + j + 1] += save_int[i + j] / 10;
      }
      save_int[i + j] = save_int[i + j] % 10;
    }
  }
  int count_zero = 0;
  for (int t = len - 1; t >= 0; --t) {
    if (save_int[t] != 0) {
      break;
    }
    count_zero++;
  }
  n->digits = realloc(n->digits, (len - count_zero + 1) * sizeof(char));
  for (int x = 0; x < len - count_zero; ++x) {
    n->digits[x] = save_int[x] + '0';
  }
  n->digits[len - count_zero] = '\0';
  free(save_int);
}

void biggie_mult(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  if (n->negative == 0 && m->negative == 1) {
    n->negative = 1;
  } else if (n->negative == 1 && m->negative == 1) {
    n->negative = 0;
  }
  mult_calc(n, n->digits, m->digits);
}

bool biggie_eq(const struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  if (n->negative == 0 && m->negative == 1) {
    return false;
  } else if (n->negative == 1 && m->negative == 0) {
    return false;
  }
  if (compare(n->digits, m->digits) == 0) {
    return true;
  } else {
    return false;
  }
}

bool biggie_gt(const struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int comp = compare(n->digits, m->digits);
  if (n->negative == 0 && m->negative == 1) {
    return true;
  } else if (n->negative == 0 && m->negative == 0 && comp > 0) {
    return true;
  } else if (n->negative == 1 && m->negative == 1 && comp < 0) {
    return true;
  } else {
    return false;
  }
}

struct biggie *biggie_copy(const struct biggie *n) {
  assert(n);
  struct biggie *big = malloc(sizeof(struct biggie));
  big->negative = n->negative;
  int len = strlen(n->digits);
  big->digits = malloc((len + 1) * sizeof(char));
  for (int i = 0; i < len; ++i) {
    big->digits[i] = n->digits[i];
  }
  big->digits[len] = '\0';
  return big;
}
