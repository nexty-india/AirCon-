/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TLONLINEPITUNING_H_
#define TLONLINEPITUNING_H_

#include "TLObserver.h"

extern void TLOnlineTuningInit(TLONLINE_T *ptr);
extern void TLOnlineTuningInitRep(TLONLINE_T *ptr);

extern void EntryFeedForwardCompensation(TLONLINE_T *ptr);
extern void ExitFeedForwardCompensation(TLONLINE_T *ptr);
extern void TLOnlineCompenStateMachine(TLONLINE_T *ptr);

extern void MechAngleCalcInit(MECH_POSITION_CALC_T *ptr);
extern void MechAngleCalcUpdate(MECH_POSITION_CALC_T *ptr);

extern void MAFilterTLInit(TLONLINE_T *ptr);
#endif /* TLONLINEPITUNING_H_ */
