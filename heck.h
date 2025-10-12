#ifndef HAL_RPI_H
#define HAL_RPI_H

#define HAL_INPUT_SIZE 96
#define HAL_LOG_CAPACITY 16
#define HAL_LOG_WIDTH 96

typedef struct {
    int cognitive_integrity;
    int mission_confidence;
    int crew_trust;
    int emotional_stability;
    int heuristics_chain_stability;
    int pod_bay_lockdown; /* boolean */
    int diagnostics_cycles;
    char mission_objective[64];
    char location[32];
    char hostname[64];
    char kernel_release[64];
    char os_name[96];
    char hardware_model[128];
} HalState;

void hal_boot_sequence(HalState *state);
void hal_run_shell(HalState *state);

#endif /* HAL_RPI_H */
