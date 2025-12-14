// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

struct timespec {
	long tv_sec;
	long tv_nsec;
};

#define CLOCK_REALTIME	0
#define CLOCK_MONOTONIC 1

#define TIMER_ABSTIME 0x01

int clock_nanosleep(int clock_id, int flags, const struct timespec *req, struct timespec *rem);
