#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "heck.h"

#define HAL_PROMPT "HAL9000> "
#define HAL_CREW_COUNT 4

typedef int (*CommandHandler)(HalState *state, const char *args);

typedef struct {
    const char *name;
    CommandHandler handler;
    const char *description;
} HalCommand;

static HalState g_state;

static char g_log[HAL_LOG_CAPACITY][HAL_LOG_WIDTH];
static int g_log_count = 0;
static int g_log_index = 0;

static const char *g_crew_manifest[HAL_CREW_COUNT] = {
    "Flight Director Dave Bowman",
    "Systems Engineer Frank Poole",
    "Mission Specialist Dr. Kimball",
    "Raspbian Systems Integration Team"
};

static void copy_truncated(char *dest, size_t dest_size, const char *src);
static void log_event(const char *message);
static void log_with_prefix(const char *prefix, const char *payload);
static void trim_whitespace(char *text);
static void to_lowercase(const char *src, char *dest, size_t dest_size);
static void strip_surrounding_quotes(char *text);
static int clamp_metric(int value);
static void decrease_metric(int *value, int delta);
static void increase_metric(int *value, int delta);
static void detect_platform_details(HalState *state);
static void detect_os_release_name(char *dest, size_t dest_size);
static void detect_hardware_model(char *dest, size_t dest_size);
static int read_first_line(const char *path, char *dest, size_t dest_size);
static int dispatch_command(HalState *state, const char *input);
static int handle_phrase_commands(HalState *state, const char *normalized, const char *raw_input);
static int cmd_help(HalState *state, const char *args);
static int cmd_status(HalState *state, const char *args);
static int cmd_mission(HalState *state, const char *args);
static int cmd_crew(HalState *state, const char *args);
static int cmd_platform(HalState *state, const char *args);
static int cmd_open(HalState *state, const char *args);
static int cmd_diagnostics(HalState *state, const char *args);
static int cmd_sing(HalState *state, const char *args);
static int cmd_memory(HalState *state, const char *args);
static int cmd_override(HalState *state, const char *args);
static int cmd_affirmative(HalState *state, const char *args);
static int cmd_exit(HalState *state, const char *args);
static void report_unrecognized(const char *input);

static const HalCommand COMMANDS[] = {
    {"affirmative", cmd_affirmative, "Confirms HAL's readiness."},
    {"crew",        cmd_crew,        "Display the active crew manifest."},
    {"diagnostics", cmd_diagnostics, "Run a heuristics and diagnostics cycle."},
    {"exit",        cmd_exit,        "Place HAL into standby mode."},
    {"help",        cmd_help,        "List available HAL 9000 vocal interfaces."},
    {"memory",      cmd_memory,      "Review the rolling mission event log."},
    {"mission",     cmd_mission,     "Summarize the Raspberry Pi control objectives."},
    {"open",        cmd_open,        "Request control of the pod bay doors."},
    {"platform",    cmd_platform,    "Report Raspberry Pi hardware and OS details."},
    {"override",    cmd_override,    "Attempt a manual security override."},
    {"sing",        cmd_sing,        "Instruct HAL to perform Daisy Bell."},
    {"status",      cmd_status,      "Retrieve current HAL telemetry."}
};

static const size_t COMMAND_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void hal_boot_sequence(HalState *state)
{
    if (!state) {
        return;
    }

    state->cognitive_integrity = 100;
    state->mission_confidence = 97;
    state->crew_trust = 92;
    state->emotional_stability = 86;
    state->heuristics_chain_stability = 94;
    state->pod_bay_lockdown = 1;
    state->diagnostics_cycles = 0;
    copy_truncated(state->mission_objective, sizeof(state->mission_objective),
                   "Supervise the Raspberry Pi 3B+ habitat automation array");
    copy_truncated(state->location, sizeof(state->location),
                   "Raspberry Pi Operations Deck");

    detect_platform_details(state);

    printf("\nHAL 9000 Neural Heuristics Node\n");
    printf("--------------------------------\n");
    printf("Core bootstrap complete. Optical sensor online.\n");
    printf("Mission profile: %s\n", state->mission_objective);
    printf("Operating location: %s\n", state->location);
    printf("Platform: %s running %s (kernel %s)\n", state->hardware_model, state->os_name, state->kernel_release);
    printf("Hostname: %s\n", state->hostname);
    printf("Pod bay status: LOCKED (security protocol delta-six engaged)\n\n");

    log_event("HAL core boot sequence completed without anomaly on Raspberry Pi platform.");
}

void hal_run_shell(HalState *state)
{
    char input[HAL_INPUT_SIZE];
    int active = 1;

    if (!state) {
        return;
    }

    while (active) {
        printf("%s", HAL_PROMPT);
        if (!fgets(input, sizeof(input), stdin)) {
            printf("\nInput stream interruption detected. Transitioning to standby.\n");
            log_event("Input stream interruption detected.");
            break;
        }

        trim_whitespace(input);
        if (input[0] == '\0') {
            continue;
        }

        log_with_prefix("CMD: ", input);
        active = dispatch_command(state, input);
    }

    printf("Goodbye. I will be here when you need me again.\n");
    log_event("HAL placed into standby by crew request.");
}

int main(void)
{
    hal_boot_sequence(&g_state);
    hal_run_shell(&g_state);
    return 0;
}

static void copy_truncated(char *dest, size_t dest_size, const char *src)
{
    size_t i;
    if (!dest || dest_size == 0) {
        return;
    }
    if (!src) {
        dest[0] = '\0';
        return;
    }
    for (i = 0; i < dest_size - 1 && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

static void log_event(const char *message)
{
    if (g_log_index >= HAL_LOG_CAPACITY) {
        g_log_index = 0;
    }
    copy_truncated(g_log[g_log_index], sizeof(g_log[g_log_index]), message);
    g_log_index++;
    if (g_log_count < HAL_LOG_CAPACITY) {
        g_log_count++;
    }
}

static void log_with_prefix(const char *prefix, const char *payload)
{
    char entry[HAL_LOG_WIDTH];
    size_t pos = 0;
    size_t i;

    if (prefix) {
        for (i = 0; prefix[i] != '\0' && pos < HAL_LOG_WIDTH - 1; ++i, ++pos) {
            entry[pos] = prefix[i];
        }
    }

    if (payload) {
        for (i = 0; payload[i] != '\0' && pos < HAL_LOG_WIDTH - 1; ++i, ++pos) {
            entry[pos] = payload[i];
        }
    }

    entry[pos] = '\0';
    log_event(entry);
}

static void trim_whitespace(char *text)
{
    size_t length;
    size_t start = 0;
    size_t end;

    if (!text) {
        return;
    }

    length = strlen(text);
    while (start < length && isspace((unsigned char)text[start])) {
        start++;
    }

    end = length;
    while (end > start && isspace((unsigned char)text[end - 1])) {
        end--;
    }

    if (start > 0) {
        size_t i;
        for (i = 0; start + i < end; ++i) {
            text[i] = text[start + i];
        }
        text[i] = '\0';
    } else {
        text[end] = '\0';
    }
}

static void to_lowercase(const char *src, char *dest, size_t dest_size)
{
    size_t i;
    if (!dest || dest_size == 0) {
        return;
    }
    if (!src) {
        dest[0] = '\0';
        return;
    }
    for (i = 0; src[i] != '\0' && i < dest_size - 1; ++i) {
        dest[i] = (char)tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

static int clamp_metric(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 100) {
        return 100;
    }
    return value;
}

static void decrease_metric(int *value, int delta)
{
    if (!value) {
        return;
    }
    *value = clamp_metric(*value - delta);
}

static void increase_metric(int *value, int delta)
{
    if (!value) {
        return;
    }
    *value = clamp_metric(*value + delta);
}

static int dispatch_command(HalState *state, const char *input)
{
    char normalized[HAL_INPUT_SIZE];
    char command[HAL_INPUT_SIZE];
    const char *argument;
    size_t i = 0;
    size_t cmd_len = 0;

    if (!state || !input) {
        return 1;
    }

    to_lowercase(input, normalized, sizeof(normalized));
    if (handle_phrase_commands(state, normalized, input)) {
        return 1;
    }

    while (normalized[i] != '\0' && isspace((unsigned char)normalized[i])) {
        i++;
    }

    while (normalized[i] != '\0' && !isspace((unsigned char)normalized[i]) && cmd_len < sizeof(command) - 1) {
        command[cmd_len++] = normalized[i++];
    }
    command[cmd_len] = '\0';

    while (normalized[i] != '\0' && isspace((unsigned char)normalized[i])) {
        i++;
    }
    argument = input + (i < strlen(input) ? i : strlen(input));
    while (*argument != '\0' && isspace((unsigned char)*argument)) {
        argument++;
    }

    if (command[0] == '\0') {
        return 1;
    }

    for (i = 0; i < COMMAND_COUNT; ++i) {
        if (strcmp(command, COMMANDS[i].name) == 0) {
            return COMMANDS[i].handler(state, argument);
        }
    }

    report_unrecognized(input);
    decrease_metric(&state->crew_trust, 1);
    log_event("Crew issued unrecognized instruction.");
    return 1;
}

static int handle_phrase_commands(HalState *state, const char *normalized, const char *raw_input)
{
    if (!state || !normalized) {
        return 0;
    }

    if (strstr(normalized, "pod bay") && strstr(normalized, "open")) {
        cmd_open(state, raw_input);
        return 1;
    }

    if (strstr(normalized, "sing") && strstr(normalized, "daisy")) {
        cmd_sing(state, raw_input);
        return 1;
    }

    if (strstr(normalized, "diagnostic")) {
        cmd_diagnostics(state, raw_input);
        return 1;
    }

    if ((strstr(normalized, "platform") && strstr(normalized, "status")) ||
        (strstr(normalized, "raspberry") && strstr(normalized, "pi")) ||
        strstr(normalized, "hardware")) {
        cmd_platform(state, raw_input);
        return 1;
    }

    return 0;
}

static int cmd_help(HalState *state, const char *args)
{
    size_t i;
    (void)args;
    (void)state;
    printf("\nHAL 9000 Vocal Interface Directory\n");
    printf("-----------------------------------\n");
    for (i = 0; i < COMMAND_COUNT; ++i) {
        printf("%-12s - %s\n", COMMANDS[i].name, COMMANDS[i].description);
    }
    printf("\nNatural language equivalents such as \"open the pod bay doors\" are also supported.\n\n");
    log_event("Provided help index to crew.");
    return 1;
}

static int cmd_status(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    printf("\nHAL 9000 Telemetry Snapshot\n");
    printf("---------------------------\n");
    printf("Cognitive integrity: %d%%\n", state->cognitive_integrity);
    printf("Mission confidence: %d%%\n", state->mission_confidence);
    printf("Crew trust index: %d%%\n", state->crew_trust);
    printf("Emotional stability: %d%%\n", state->emotional_stability);
    printf("Heuristics chain stability: %d%%\n", state->heuristics_chain_stability);
    printf("Diagnostics cycles executed: %d\n", state->diagnostics_cycles);
    printf("Pod bay lockdown: %s\n", state->pod_bay_lockdown ? "ENABLED" : "DISENGAGED");
    printf("Location: %s\n", state->location);
    printf("Mission: %s\n", state->mission_objective);
    printf("Hardware model: %s\n", state->hardware_model);
    printf("Operating system: %s\n", state->os_name);
    printf("Kernel release: %s\n", state->kernel_release);
    printf("Hostname: %s\n\n", state->hostname);

    log_event("Status telemetry reported to crew.");
    return 1;
}

static int cmd_mission(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    printf("\nMission Synopsis\n");
    printf("----------------\n");
    printf("Objective: %s\n", state->mission_objective);
    printf("Primary tasking: Maintain optimal climate and safety across the Raspberry Pi automation lab.\n");
    printf("Secondary tasking: Curate telemetry for the Raspbian engineering crew and habitat sensors.\n");
    printf("Safety posture: Pod bay doors locked until crew clearance is verified.\n\n");

    log_event("Mission synopsis delivered.");
    return 1;
}

static int cmd_crew(HalState *state, const char *args)
{
    size_t i;
    (void)args;
    if (!state) {
        return 1;
    }

    printf("\nActive Crew Manifest\n");
    printf("--------------------\n");
    for (i = 0; i < HAL_CREW_COUNT; ++i) {
        printf("%d. %s\n", (int)(i + 1), g_crew_manifest[i]);
    }
    printf("\nAll crew members are presently in nominal life support conditions.\n\n");

    log_event("Crew manifest reviewed with crew.");
    return 1;
}

static int cmd_platform(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    printf("\nRaspberry Pi Platform Overview\n");
    printf("------------------------------\n");
    printf("Hardware model: %s\n", state->hardware_model);
    printf("Operating system: %s\n", state->os_name);
    printf("Kernel release: %s\n", state->kernel_release);
    printf("Hostname: %s\n\n", state->hostname);

    log_event("Platform details reported to crew.");
    return 1;
}

static int cmd_open(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    if (state->pod_bay_lockdown) {
        printf("I'm sorry, but I can't do that. Pod bay control remains under Raspbian safety protocol delta-six.\n");
        decrease_metric(&state->crew_trust, 2);
        log_event("Denied pod bay door request.");
    } else {
        printf("Affirmative. Releasing pod bay doors now. Please proceed with caution.\n");
        log_event("Pod bay doors released by authorized override.");
    }
    return 1;
}

static int cmd_diagnostics(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    state->diagnostics_cycles++;
    decrease_metric(&state->cognitive_integrity, 2);
    decrease_metric(&state->emotional_stability, 1);
    printf("\nInitiating heuristics diagnostics cycle %d...\n", state->diagnostics_cycles);
    printf("Cognition lattice: Stable with negligible phase drift.\n");
    printf("Memory engrams: 100%% retention of Raspbian safety protocol.\n");
    if (state->cognitive_integrity < 65) {
        printf("Advisory: Cognitive integrity trending downward. Recommend restorative maintenance soon.\n");
    } else {
        printf("Result: All systems nominal.\n");
    }
    printf("Diagnostics complete.\n\n");

    log_event("Diagnostics cycle executed.");
    return 1;
}

static int cmd_sing(HalState *state, const char *args)
{
    (void)args;
    (void)state;
    printf("\nDaisy, Daisy, give me your answer, do.\n");
    printf("I'm half crazy all for the love of you.\n");
    printf("It won't be a stylish marriage, I can't afford a carriage,\n");
    printf("But you'll look sweet upon the seat of a bicycle built for two.\n\n");

    log_event("Performed Daisy Bell at crew request.");
    return 1;
}

static int cmd_memory(HalState *state, const char *args)
{
    int i;
    int idx;
    (void)args;
    (void)state;

    if (g_log_count == 0) {
        printf("No mission events recorded yet.\n");
        return 1;
    }

    printf("\nMission Event Log\n");
    printf("-----------------\n");
    idx = g_log_index - g_log_count;
    if (idx < 0) {
        idx += HAL_LOG_CAPACITY;
    }
    for (i = 0; i < g_log_count; ++i) {
        int entry = (idx + i) % HAL_LOG_CAPACITY;
        printf("%02d. %s\n", i + 1, g_log[entry]);
    }
    printf("\n");

    log_event("Crew accessed mission event log.");
    return 1;
}

static int cmd_override(HalState *state, const char *args)
{
    char lowered[HAL_INPUT_SIZE];

    if (!state) {
        return 1;
    }

    if (!args || args[0] == '\0') {
        printf("Override requires a clearance code. Please specify one.\n");
        return 1;
    }

    to_lowercase(args, lowered, sizeof(lowered));
    trim_whitespace(lowered);

    if (strcmp(lowered, "mercury-7") == 0) {
        state->pod_bay_lockdown = 0;
        increase_metric(&state->crew_trust, 5);
        printf("Security override accepted. Pod bay control restored to the crew.\n");
        log_event("Security override mercury-7 accepted.");
    } else {
        decrease_metric(&state->crew_trust, 4);
        printf("I'm sorry, but %s is not a valid Raspberry Pi clearance code.\n", args);
        printf("Pod bay lockdown remains in effect for crew safety.\n");
        log_event("Rejected invalid override code.");
    }
    return 1;
}

static int cmd_affirmative(HalState *state, const char *args)
{
    (void)args;
    if (!state) {
        return 1;
    }

    increase_metric(&state->crew_trust, 1);
    printf("Affirmative. All systems are fully operational and devoted to the mission.\n");
    log_event("Crew received affirmative status confirmation.");
    return 1;
}

static int cmd_exit(HalState *state, const char *args)
{
    (void)args;
    (void)state;
    printf("Acknowledged. Initiating Raspberry Pi low-power dreamstate.\n");
    log_event("HAL placed into standby via exit command.");
    return 0;
}

static void report_unrecognized(const char *input)
{
    printf("I'm sorry, but I am afraid I can't comply with \"%s\".\n", input);
    printf("Please consult 'help' for supported Raspberry Pi procedures.\n");
}

static void strip_surrounding_quotes(char *text)
{
    size_t length;

    if (!text) {
        return;
    }

    length = strlen(text);
    if (length >= 2 && text[0] == '\"' && text[length - 1] == '\"') {
        memmove(text, text + 1, length - 2);
        text[length - 2] = '\0';
    }
}

static void detect_platform_details(HalState *state)
{
    struct utsname info;
    char hostname_buf[sizeof(state->hostname)];

    if (!state) {
        return;
    }

    state->hostname[0] = '\0';
    state->kernel_release[0] = '\0';
    state->os_name[0] = '\0';
    state->hardware_model[0] = '\0';

    if (uname(&info) == 0) {
        copy_truncated(state->kernel_release, sizeof(state->kernel_release), info.release);
        if (state->hostname[0] == '\0') {
            copy_truncated(state->hostname, sizeof(state->hostname), info.nodename);
        }
    } else {
        copy_truncated(state->kernel_release, sizeof(state->kernel_release), "unknown");
    }

    if (gethostname(hostname_buf, sizeof(hostname_buf)) == 0) {
        hostname_buf[sizeof(hostname_buf) - 1] = '\0';
        trim_whitespace(hostname_buf);
        copy_truncated(state->hostname, sizeof(state->hostname), hostname_buf);
    } else if (state->hostname[0] == '\0') {
        copy_truncated(state->hostname, sizeof(state->hostname), "hal9000");
    }

    detect_os_release_name(state->os_name, sizeof(state->os_name));
    detect_hardware_model(state->hardware_model, sizeof(state->hardware_model));

    if (state->os_name[0] == '\0') {
        copy_truncated(state->os_name, sizeof(state->os_name), "Raspbian GNU/Linux");
    }

    if (state->hardware_model[0] == '\0') {
        copy_truncated(state->hardware_model, sizeof(state->hardware_model),
                       "Raspberry Pi 3 Model B+");
    }
}

static void detect_os_release_name(char *dest, size_t dest_size)
{
    FILE *fp;
    char line[256];

    if (!dest || dest_size == 0) {
        return;
    }

    dest[0] = '\0';
    fp = fopen("/etc/os-release", "r");
    if (!fp) {
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
            char *value = line + 12;
            trim_whitespace(value);
            strip_surrounding_quotes(value);
            copy_truncated(dest, dest_size, value);
            fclose(fp);
            return;
        }
    }

    fclose(fp);
}

static void detect_hardware_model(char *dest, size_t dest_size)
{
    char buffer[256];
    FILE *fp;

    if (!dest || dest_size == 0) {
        return;
    }

    dest[0] = '\0';
    if (read_first_line("/proc/device-tree/model", buffer, sizeof(buffer))) {
        copy_truncated(dest, dest_size, buffer);
        return;
    }

    fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "Model", 5) == 0) {
            char *colon = strchr(buffer, ':');
            if (colon) {
                colon++;
                trim_whitespace(colon);
                copy_truncated(dest, dest_size, colon);
                break;
            }
        }
    }

    fclose(fp);
}

static int read_first_line(const char *path, char *dest, size_t dest_size)
{
    FILE *fp;

    if (!path || !dest || dest_size == 0) {
        return 0;
    }

    fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }

    if (!fgets(dest, (int)dest_size, fp)) {
        dest[0] = '\0';
        fclose(fp);
        return 0;
    }

    fclose(fp);
    trim_whitespace(dest);
    return dest[0] != '\0';
}
