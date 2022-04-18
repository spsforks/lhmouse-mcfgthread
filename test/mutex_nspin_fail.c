/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/mutex.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static _MCF_mutex mutex;

int
main(void)
  {
    assert(mutex.__locked == 0);
    assert(mutex.__nspin == 0);
    assert(mutex.__nspin_fail == 0);

    int64_t timeout = -100;
    assert(_MCF_mutex_lock(&mutex, &timeout) == 0);
    assert(mutex.__locked == 1);
    assert(mutex.__nspin == 0);
    assert(mutex.__nspin_fail == 0);

    for(size_t count = 1;  count <= __MCF_MUTEX_NSPIN_FAIL_M;  ++count) {
      printf("try failing: %d\n", (int) count);
      assert(_MCF_mutex_lock(&mutex, &timeout) == -1);
      assert(mutex.__locked == 1);
      assert(mutex.__nspin == 0);
      assert(mutex.__nspin_fail == count);
    }

    printf("try failing: final\n");
    assert(_MCF_mutex_lock(&mutex, &timeout) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__nspin == 0);
    assert(mutex.__nspin_fail == __MCF_MUTEX_NSPIN_FAIL_M);

    for(size_t count = __MCF_MUTEX_NSPIN_FAIL_M;  count >= 1;  --count) {
      printf("try succeeding: %d\n", (int) count);
      _MCF_mutex_unlock(&mutex);
      assert(mutex.__locked == 0);
      assert(_MCF_mutex_lock(&mutex, &timeout) == 0);
      assert(mutex.__nspin == 0);
      assert(mutex.__nspin_fail == count - 1);
      timeout /= 2;
    }

    printf("try succeeding: final\n");
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(_MCF_mutex_lock(&mutex, &timeout) == 0);
    assert(mutex.__nspin == 0);
    assert(mutex.__nspin_fail == 0);
  }
