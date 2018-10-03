#include <iostream>
#include <cassert>
#include <vector>
//#include <FMOD/fmod.hpp>
//#include <FMOD/fmod_errors.h>
#include <unordered_map>

//#include "State.h"
//#include "Main.h"
#include "Audio.h"

const nullptr_t ThisIsARandomSymbolJustToSilenceLinkerWarnings = nullptr;

// As you can see, this unused code is all gutted out. In the future it needs to be
// rewritten from this shoddy piece of garbage (holla CPE 476 flyin' fast) to something
// we can use well.

/*typedef enum {
   Warning,
   Error,
   Fatal
} ErrorLevel;

namespace {
   const int MAX_CHANNELS = 64;
   
   bool check(FMOD_RESULT result, const std::string &message, ErrorLevel errorLevel = Warning) {
      if (result != FMOD_OK) {
         std::string level;
         switch (errorLevel) {
            case Warning:
               level = "warning";
               break;
            case Error:
               level = "error";
               break;
            case Fatal:
               level = "fatal";
               break;
            default:
               level = "invalid";
         }
         
         std::cerr << "FMOD " << level << " while doing " << message << ": " <<FMOD_ErrorString(result) << std::endl;
      }
      
      return result == FMOD_OK;
   }
}; // namespace

FMOD::System *audio_system = nullptr;
FMOD::ChannelGroup *sounds = nullptr;
std::vector<Music *> musics;
std::unordered_map<std::string, FMOD::Sound*> soundMap;

float audio_latency = 0;
float audio_getLatency() {
   return audio_latency;
}

void audio_init() {
   soundMap.clear();
   
   assert(audio_system == nullptr);
   
   if (!check(FMOD::System_Create(&audio_system), "system creation", Fatal)) {
      audio_release();
      return;
   }
   
   unsigned int version;
   if (!check(audio_system->getVersion(&version), "version check", Fatal)) {
      return;
   }
   
   if (version != FMOD_VERSION) {
      std::cerr << "FMOD lib version " << version << " doesn't match header version " << FMOD_VERSION << std::endl;
      return;
   }
   
   check(audio_system->setDSPBufferSize(256, 10), "buffer size set");

   FMOD_RESULT result;
   unsigned int blocksize;
   int numblocks, samplerate;
   float ms;

   result = audio_system->getDSPBufferSize(&blocksize, &numblocks);
   result = audio_system->getSoftwareFormat(&samplerate, 0, 0);

   ms = (float)blocksize * 1000.0f / (float)samplerate;

   audio_latency = ms * ((float)numblocks - 1.5f);

   printf("Mixer blocksize        = %.02f ms\n", ms);
   printf("Mixer Total buffersize = %.02f ms\n", ms * numblocks);
   printf("Mixer Average Latency  = %.02f ms\n", ms * ((float)numblocks - 1.5f));
   
   if (!check(audio_system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr), "audio system initialization", Fatal)) {
      audio_release();
      return;
   }
   
   check(audio_system->createChannelGroup("music", &sounds), "channel group creation");
   //check(sounds->setPaused(PAUSED), "sound pause setup");
}

void audio_setPaused(bool paused) {
   assert(audio_system != nullptr);
   assert(sounds != nullptr);
   
   sounds->setPaused(paused);
}

void audio_update() {
   assert(audio_system != nullptr);
   check(audio_system->update(), "audio system update");
   
   std::vector<Music *>::iterator it;
   for(it = musics.begin(); it < musics.end(); it ++) {
      (*it)->update();
   }
}

void audio_release() {
   assert(audio_system == nullptr);
   
   sounds->stop();
   sounds->release();
}

FMOD::Sound *audio_load_sound(std::string fileName) {
   assert(audio_system != nullptr);
   
   if (soundMap[fileName])
      return soundMap[fileName];
   
   FMOD::Sound *sound;
   if (!check(audio_system->createStream(fileName.c_str(), FMOD_DEFAULT, nullptr, &sound), "loading sound", Error)) {
      return nullptr;
   }
   
   std::cout << "Loaded sound " << fileName << std::endl;

   soundMap[fileName] = sound;

   return sound;
}

Music *audio_load_music(std::string fileName, Beat bpm, bool looping) {
   FMOD::Sound *sound = audio_load_sound(fileName);
   check(sound->setMode(FMOD_DEFAULT | (looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF)), "sound looping", Error);
   check(sound->setLoopCount(-1), "sound->setLoopCount(-1)", Error);
   
   Music *music = new Music(sound, bpm);
   musics.push_back(music);
   return music;
}

void audio_play_music(Music *music) {
   assert(audio_system != nullptr);
   
   if (!music->getSound()) {
      std::cerr << "No sound registered to this music" << std::endl;
      return;
   }
   
   // Start music if it exists already
   FMOD::Channel *channel = music->getChannel();
   if (!channel) {
      check(audio_system->playSound(music->getSound(), sounds, true, &channel), "music play");
      
      if (!channel) {
         std::cerr << "Unable to play music" << std::endl;
         return;
      }
   }
   
   check(channel->setPaused(false), "sound unpause");
   music->setChannel(channel);
}

void audio_play_sound(std::string fileName) {
   assert(audio_system != nullptr);
   
   FMOD::Sound *sound = audio_load_sound(fileName);
   assert(sound != nullptr); // Sound file may not exist
   
   FMOD::Channel *channel;
   check(audio_system->playSound(sound, sounds, true, &channel), "sound play");
   check(channel->setVolume(0.20f), "Sound volume set");

   channel->setPaused(false);
}

Music::Music(FMOD::Sound *s, Beat _bpm) : channel(nullptr), sound(s), beat(0), bpm(_bpm) {
   check(s->getLength(&length, FMOD_TIMEUNIT_MS), "length get");
   
   check(audio_system->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp), "DSP creation");
   check(dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_RECT), "window type setting");
   check(dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, 4096), "window size setting");
   
   memset(spectrum, 0, sizeof(spectrum));
};

float Music::getProgress() {
   unsigned int position;
   check(channel->getPosition(&position, FMOD_TIMEUNIT_MS), "time check");
   
   return (float) position / length;
}

void Music::update() {
   // Compute frequency spectrum
   float val;
   char _s[256];
   unsigned int len;
   FMOD_DSP_PARAMETER_FFT *data = 0;
   int rate;
   check(audio_system->getSoftwareFormat(&rate , 0, 0), "get software format");
   check(dsp->getParameterFloat(FMOD_DSP_FFT_DOMINANT_FREQ, &val, 0, 0), "get dominant");
   check(dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&data, &len, _s, 256), "get spectrum");
   
   if (data->length > 0) {
      int average_samples = data->length / data->numchannels / MAX_SPECTRA;
      int index = 0;
      for (int s = 0; s < MAX_SPECTRA; s ++) {
         spectrum[s] = 0;
         
         int ds = average_samples * s * 2 / MAX_SPECTRA;
         for (int i = 0; i < ds; i ++) {
            for (int chan = 0; chan < data->numchannels; chan ++) {
               // arbitrary cutoff to filter out noise
               if (data->spectrum[chan][index] > 0.0001f) {
                  spectrum[s] += data->spectrum[chan][index];
               }
            }
            
            index ++;
         }
         
         if (spectrum[s] < 0.001f) {
            spectrum[s] = 0;
         }
      }
   
      currentSpectrum = (currentSpectrum + 1) % SAMPLES_PER_SPECTRUM;
   }

   // Calculate beat (more accurate here)
   Beat position;
   channel->getPosition(&position, FMOD_TIMEUNIT_MS);

   Beat currentBeat = position * bpm / 60000;
   beat_offset = position - (currentBeat - 1) * 60000 / bpm;
   if (currentBeat != beat) {
      beat = currentBeat;

      //getCurrentState()->send("beat", &beat);
   }
}

void Music::rewind(Beat amount) {
   Beat position;
   check(channel->getPosition(&position, FMOD_TIMEUNIT_MS), "get channel position");

   // Convert to MS
   Beat d_pos = amount * 60000 / bpm;
   if (position < d_pos) {
      position = 0;
      beat = 0;
      beat_offset = 0;
   }
   else {
      position -= d_pos;
      beat -= amount;
   }

   check(channel->setPosition(position, FMOD_TIMEUNIT_MS), "set channel position");
}

float Music::getSample(int index, int totalSpectra) {
   assert(totalSpectra <= MAX_SPECTRA);
   assert(index < totalSpectra);
   
   float avg = 0;
   // Consolidate spectrum groups
   int scale = MAX_SPECTRA / totalSpectra;
   for (int s = scale * index; s < scale * (index + 1); s ++) {
      for (int i = 0; i < SAMPLES_PER_SPECTRUM; i ++) {
         avg += spectrum[s];
      }
   }
   
   return avg / (scale * SAMPLES_PER_SPECTRUM);
}

void Music::getSamples(float *samples, int number) {
   assert(number <= MAX_SPECTRA);
   
   // Zero the memory
   memset(samples, 0, sizeof(float) * number);
   
   // Consolidate spectrum groups
   int scale = MAX_SPECTRA / number;
   for (int s = 0; s < MAX_SPECTRA; s ++) {
//      for (int i = 0; i < SAMPLES_PER_SPECTRUM; i ++) {
         samples[s * number / MAX_SPECTRA] += spectrum[s];
//      }
   }
   
   for (int s = 0; s < number; s ++)
      samples[s] /= scale * SAMPLES_PER_SPECTRUM;
}

void Music::play() {
   if (channel == nullptr) {
      audio_play_music(this);
      check(channel->addDSP(0, dsp), "DSP chaining");
      check(dsp->setActive(true), "DSP activation");
   }
   else {
      check(channel->setPaused(false), "Unpausing channel");
   }
}

void Music::pause() {
   check(channel->setPaused(true), "Pausing channel");
}

void Music::stop() {
   pause();
   check(channel->setPosition(0, FMOD_TIMEUNIT_MS), "Resetting music");
}*/
