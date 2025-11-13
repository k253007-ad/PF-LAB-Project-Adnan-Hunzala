#ifndef RAYLIB_VARIED_SOUND_H
#define RAYLIB_VARIED_SOUND_H

#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define VARIED_MAX_VOICES 6

typedef struct {
    Sound instance;
    float startVolume;
    double startTime;
    bool active;              
} VariedVoice;


typedef struct {
    Sound raw;                
    VariedVoice voices[VARIED_MAX_VOICES];  
    
    
    float basePitch;          
    float pitchRange;         
    float volRangeDB;         
    float panRangeDeg;        
    
    
    float minInterval;        
    float fadeInTime;         
    double lastPlayTime;      
    
    int nextVoiceIndex;       
} VariedSound;


static inline float RandF(float min, float max) {
    return min + (max - min) * (rand() / (float)RAND_MAX);
}


static inline void InitVariedSound(VariedSound* vs, Sound source) {
    memset(vs, 0, sizeof(VariedSound));
    
    vs->raw = source;
    vs->basePitch = 1.0f;
    vs->pitchRange = 0.07f;     
    vs->volRangeDB = 2.0f;      
    vs->panRangeDeg = 15.0f;    
    vs->minInterval = 0.08f;    
    vs->fadeInTime = 0.015f;    
    vs->lastPlayTime = -999.0;
    vs->nextVoiceIndex = 0;
    
    
    for (int i = 0; i < VARIED_MAX_VOICES; i++) {
        vs->voices[i].instance = source;  
        vs->voices[i].active = false;
        vs->voices[i].startVolume = 1.0f;
        vs->voices[i].startTime = 0.0;
    }
}


static inline VariedVoice* AcquireVoice(VariedSound* vs) {
    
    for (int i = 0; i < VARIED_MAX_VOICES; i++) {
        if (!vs->voices[i].active || !IsSoundPlaying(vs->voices[i].instance)) {
            return &vs->voices[i];
        }
    }
    
    
    int idx = vs->nextVoiceIndex;
    vs->nextVoiceIndex = (vs->nextVoiceIndex + 1) % VARIED_MAX_VOICES;
    StopSound(vs->voices[idx].instance);
    return &vs->voices[idx];
}


static inline void PlayVariedSound(VariedSound* vs) {
    double now = GetTime();
    
    
    if (now - vs->lastPlayTime < vs->minInterval) return;
    vs->lastPlayTime = now;
    
    
    VariedVoice* voice = AcquireVoice(vs);
    if (!voice) return;
    
    
    float pitch = vs->basePitch + RandF(-vs->pitchRange, vs->pitchRange);
    SetSoundPitch(voice->instance, pitch);
    
    
    float volumeDB = RandF(-vs->volRangeDB, vs->volRangeDB);
    float volume = powf(10.0f, volumeDB / 20.0f);  
    voice->startVolume = volume;
    
    
    float pan = sinf(RandF(-vs->panRangeDeg, vs->panRangeDeg) * DEG2RAD);
    SetSoundPan(voice->instance, pan);
    
    
    if (vs->fadeInTime > 0.0f) {
        SetSoundVolume(voice->instance, 0.0f);
    } else {
        SetSoundVolume(voice->instance, volume);
    }
    
    
    PlaySound(voice->instance);
    voice->active = true;
    voice->startTime = now;
}


static inline void UpdateVariedSound(VariedSound* vs) {
    double now = GetTime();
    
    for (int i = 0; i < VARIED_MAX_VOICES; i++) {
        VariedVoice* voice = &vs->voices[i];
        
        if (!voice->active) continue;
        
        
        if (!IsSoundPlaying(voice->instance)) {
            voice->active = false;
            continue;
        }
        
        
        if (vs->fadeInTime > 0.0f) {
            float elapsed = (float)(now - voice->startTime);
            if (elapsed < vs->fadeInTime) {
                
                float fadeProgress = elapsed / vs->fadeInTime;
                SetSoundVolume(voice->instance, voice->startVolume * fadeProgress);
            } else {
                
                SetSoundVolume(voice->instance, voice->startVolume);
            }
        }
    }
}


static inline void PlaySoundNormal(Sound sound) {
    SetSoundPitch(sound, 1.0f);
    SetSoundVolume(sound, 1.0f);
    SetSoundPan(sound, 0.5f);  
    PlaySound(sound);
}


typedef enum {
    VARIED_PRESET_SUBTLE,       
    VARIED_PRESET_FOOTSTEPS,    
    VARIED_PRESET_IMPACTS,      
    VARIED_PRESET_GUNSHOTS,     
} VariedPreset;

static inline void InitVariedSoundPreset(VariedSound* vs, Sound source, VariedPreset preset) {
    InitVariedSound(vs, source);
    
    switch (preset) {
        case VARIED_PRESET_SUBTLE:
            vs->pitchRange = 0.02f;      
            vs->volRangeDB = 1.0f;       
            vs->panRangeDeg = 5.0f;      
            vs->minInterval = 0.05f;
            break;
            
        case VARIED_PRESET_FOOTSTEPS:
            vs->pitchRange = 0.08f;      
            vs->volRangeDB = 2.5f;       
            vs->panRangeDeg = 20.0f;     
            vs->minInterval = 0.15f;     
            break;
            
        case VARIED_PRESET_IMPACTS:
            vs->pitchRange = 0.15f;      
            vs->volRangeDB = 4.0f;       
            vs->panRangeDeg = 25.0f;     
            vs->minInterval = 0.0f;      
            break;
            
        case VARIED_PRESET_GUNSHOTS:
            vs->pitchRange = 0.05f;      
            vs->volRangeDB = 2.0f;       
            vs->panRangeDeg = 10.0f;     
            vs->minInterval = 0.0f;      
            vs->fadeInTime = 0.0f;       
            break;
    }
}

#endif 