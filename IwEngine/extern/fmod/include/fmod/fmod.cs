/* ======================================================================================== */
/* FMOD Core API - C# wrapper.                                                              */
/* Copyright (c), Firelight Technologies Pty, Ltd. 2004-2020.                               */
/*                                                                                          */
/* For more detail visit:                                                                   */
/* https://fmod.com/resources/documentation-api?version=2.0&page=core-api.html              */
/* ======================================================================================== */

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace FMOD
{
    /*
        FMOD version number.  Check this against FMOD::System::getVersion / System_GetVersion
        0xaaaabbcc -> aaaa = major version number.  bb = minor version number.  cc = development version number.
    */
    public class VERSION
    {
        public const int    number = 0x00020007;
#if (UNITY_IPHONE || UNITY_TVOS || UNITY_SWITCH || UNITY_WEBGL) && !UNITY_EDITOR
        public const string dll    = "__Internal";
#elif (UNITY_PS4) && DEVELOPMENT_BUILD
        public const string dll    = "libfmodL";
#elif (UNITY_PS4) && !UNITY_EDITOR
        public const string dll    = "libfmod";
#elif (UNITY_PSP2) && !UNITY_EDITOR
        public const string dll    = "libfmodstudio";
/* Linux defines moved before the Windows define, otherwise Linux Editor tries to use Win lib when selected as build target.*/
#elif (UNITY_EDITOR_LINUX) || ((UNITY_STANDALONE_LINUX || UNITY_ANDROID || UNITY_XBOXONE || UNITY_STADIA) && DEVELOPMENT_BUILD)
        public const string dll    = "fmodL";
#elif (UNITY_EDITOR_OSX || UNITY_EDITOR_WIN) || ((UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN) && DEVELOPMENT_BUILD)
        public const string dll    = "fmodstudioL";
#elif (UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN)
        public const string dll    = "fmodstudio";
#else
        public const string dll    = "fmod";
#endif
    }

    public class CONSTANTS
    {
        public const int MAX_CHANNEL_WIDTH = 32;
        public const int MAX_LISTENERS = 8;
        public const int REVERB_MAXINSTANCES = 4;
        public const int MAX_SYSTEMS = 8;
    }

    /*
        FMOD types
    */
    public enum RESULT : int
    {
        OK,                             /* No errors. */
        ERR_BADCOMMAND,                 /* Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound). */
        ERR_CHANNEL_ALLOC,              /* Error trying to allocate a channel. */
        ERR_CHANNEL_STOLEN,             /* The specified channel has been reused to play another sound. */
        ERR_DMA,                        /* DMA Failure.  See debug output for more information. */
        ERR_DSP_CONNECTION,             /* DSP connection error.  Connection possibly caused a cyclic dependency or connected dsps with incompatible buffer counts. */
        ERR_DSP_DONTPROCESS,            /* DSP return code from a DSP process query callback.  Tells mixer not to call the process callback and therefore not consume CPU.  Use this to optimize the DSP graph. */
        ERR_DSP_FORMAT,                 /* DSP Format error.  A DSP unit may have attempted to connect to this network with the wrong format, or a matrix may have been set with the wrong size if the target unit has a specified channel map. */
        ERR_DSP_INUSE,                  /* DSP is already in the mixer's DSP network. It must be removed before being reinserted or released. */
        ERR_DSP_NOTFOUND,               /* DSP connection error.  Couldn't find the DSP unit specified. */
        ERR_DSP_RESERVED,               /* DSP operation error.  Cannot perform operation on this DSP as it is reserved by the system. */
        ERR_DSP_SILENCE,                /* DSP return code from a DSP process query callback.  Tells mixer silence would be produced from read, so go idle and not consume CPU.  Use this to optimize the DSP graph. */
        ERR_DSP_TYPE,                   /* DSP operation cannot be performed on a DSP of this type. */
        ERR_FILE_BAD,                   /* Error loading file. */
        ERR_FILE_COULDNOTSEEK,          /* Couldn't perform seek operation.  This is a limitation of the medium (ie netstreams) or the file format. */
        ERR_FILE_DISKEJECTED,           /* Media was ejected while reading. */
        ERR_FILE_EOF,                   /* End of file unexpectedly reached while trying to read essential data (truncated?). */
        ERR_FILE_ENDOFDATA,             /* End of current chunk reached while trying to read data. */
        ERR_FILE_NOTFOUND,              /* File not found. */
        ERR_FORMAT,                     /* Unsupported file or audio format. */
        ERR_HEADER_MISMATCH,            /* There is a version mismatch between the FMOD header and either the FMOD Studio library or the FMOD Low Level library. */
        ERR_HTTP,                       /* A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere. */
        ERR_HTTP_ACCESS,                /* The specified resource requires authentication or is forbidden. */
        ERR_HTTP_PROXY_AUTH,            /* Proxy authentication is required to access the specified resource. */
        ERR_HTTP_SERVER_ERROR,          /* A HTTP server error occurred. */
        ERR_HTTP_TIMEOUT,               /* The HTTP request timed out. */
        ERR_INITIALIZATION,             /* FMOD was not initialized correctly to support this function. */
        ERR_INITIALIZED,                /* Cannot call this command after System::init. */
        ERR_INTERNAL,                   /* An error occurred that wasn't supposed to.  Contact support. */
        ERR_INVALID_FLOAT,              /* Value passed in was a NaN, Inf or denormalized float. */
        ERR_INVALID_HANDLE,             /* An invalid object handle was used. */
        ERR_INVALID_PARAM,              /* An invalid parameter was passed to this function. */
        ERR_INVALID_POSITION,           /* An invalid seek position was passed to this function. */
        ERR_INVALID_SPEAKER,            /* An invalid speaker was passed to this function based on the current speaker mode. */
        ERR_INVALID_SYNCPOINT,          /* The syncpoint did not come from this sound handle. */
        ERR_INVALID_THREAD,             /* Tried to call a function on a thread that is not supported. */
        ERR_INVALID_VECTOR,             /* The vectors passed in are not unit length, or perpendicular. */
        ERR_MAXAUDIBLE,                 /* Reached maximum audible playback count for this sound's soundgroup. */
        ERR_MEMORY,                     /* Not enough memory or resources. */
        ERR_MEMORY_CANTPOINT,           /* Can't use FMOD_OPENMEMORY_POINT on non PCM source data, or non mp3/xma/adpcm data if FMOD_CREATECOMPRESSEDSAMPLE was used. */
        ERR_NEEDS3D,                    /* Tried to call a command on a 2d sound when the command was meant for 3d sound. */
        ERR_NEEDSHARDWARE,              /* Tried to use a feature that requires hardware support. */
        ERR_NET_CONNECT,                /* Couldn't connect to the specified host. */
        ERR_NET_SOCKET_ERROR,           /* A socket error occurred.  This is a catch-all for socket-related errors not listed elsewhere. */
        ERR_NET_URL,                    /* The specified URL couldn't be resolved. */
        ERR_NET_WOULD_BLOCK,            /* Operation on a non-blocking socket could not complete immediately. */
        ERR_NOTREADY,                   /* Operation could not be performed because specified sound/DSP connection is not ready. */
        ERR_OUTPUT_ALLOCATED,           /* Error initializing output device, but more specifically, the output device is already in use and cannot be reused. */
        ERR_OUTPUT_CREATEBUFFER,        /* Error creating hardware sound buffer. */
        ERR_OUTPUT_DRIVERCALL,          /* A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted. */
        ERR_OUTPUT_FORMAT,              /* Soundcard does not support the specified format. */
        ERR_OUTPUT_INIT,                /* Error initializing output device. */
        ERR_OUTPUT_NODRIVERS,           /* The output device has no drivers installed.  If pre-init, FMOD_OUTPUT_NOSOUND is selected as the output mode.  If post-init, the function just fails. */
        ERR_PLUGIN,                     /* An unspecified error has been returned from a plugin. */
        ERR_PLUGIN_MISSING,             /* A requested output, dsp unit type or codec was not available. */
        ERR_PLUGIN_RESOURCE,            /* A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback) */
        ERR_PLUGIN_VERSION,             /* A plugin was built with an unsupported SDK version. */
        ERR_RECORD,                     /* An error occurred trying to initialize the recording device. */
        ERR_REVERB_CHANNELGROUP,        /* Reverb properties cannot be set on this channel because a parent channelgroup owns the reverb connection. */
        ERR_REVERB_INSTANCE,            /* Specified instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because it is an invalid instance number or the reverb doesn't exist. */
        ERR_SUBSOUNDS,                  /* The error occurred because the sound referenced contains subsounds when it shouldn't have, or it doesn't contain subsounds when it should have.  The operation may also not be able to be performed on a parent sound. */
        ERR_SUBSOUND_ALLOCATED,         /* This subsound is already being used by another sound, you cannot have more than one parent to a sound.  Null out the other parent's entry first. */
        ERR_SUBSOUND_CANTMOVE,          /* Shared subsounds cannot be replaced or moved from their parent stream, such as when the parent stream is an FSB file. */
        ERR_TAGNOTFOUND,                /* The specified tag could not be found or there are no tags. */
        ERR_TOOMANYCHANNELS,            /* The sound created exceeds the allowable input channel count.  This can be increased using the 'maxinputchannels' parameter in System::setSoftwareFormat. */
        ERR_TRUNCATED,                  /* The retrieved string is too long to fit in the supplied buffer and has been truncated. */
        ERR_UNIMPLEMENTED,              /* Something in FMOD hasn't been implemented when it should be! contact support! */
        ERR_UNINITIALIZED,              /* This command failed because System::init or System::setDriver was not called. */
        ERR_UNSUPPORTED,                /* A command issued was not supported by this object.  Possibly a plugin without certain callbacks specified. */
        ERR_VERSION,                    /* The version number of this file format is not supported. */
        ERR_EVENT_ALREADY_LOADED,       /* The specified bank has already been loaded. */
        ERR_EVENT_LIVEUPDATE_BUSY,      /* The live update connection failed due to the game already being connected. */
        ERR_EVENT_LIVEUPDATE_MISMATCH,  /* The live update connection failed due to the game data being out of sync with the tool. */
        ERR_EVENT_LIVEUPDATE_TIMEOUT,   /* The live update connection timed out. */
        ERR_EVENT_NOTFOUND,             /* The requested event, bus or vca could not be found. */
        ERR_STUDIO_UNINITIALIZED,       /* The Studio::System object is not yet initialized. */
        ERR_STUDIO_NOT_LOADED,          /* The specified resource is not loaded, so it can't be unloaded. */
        ERR_INVALID_STRING,             /* An invalid string was passed to this function. */
        ERR_ALREADY_LOCKED,             /* The specified resource is already locked. */
        ERR_NOT_LOCKED,                 /* The specified resource is not locked, so it can't be unlocked. */
        ERR_RECORD_DISCONNECTED,        /* The specified recording driver has been disconnected. */
        ERR_TOOMANYSAMPLES,             /* The length provided exceed the allowable limit. */
    }

    public enum CHANNELCONTROL_TYPE : int
    {
        CHANNEL,
        CHANNELGROUP,
        MAX
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct VECTOR
    {
        public float x;        /* X co-ordinate in 3D space. */
        public float y;        /* Y co-ordinate in 3D space. */
        public float z;        /* Z co-ordinate in 3D space. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ATTRIBUTES_3D
    {
        public VECTOR position;
        public VECTOR velocity;
        public VECTOR forward;
        public VECTOR up;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ASYNCREADINFO
    {
        public IntPtr   handle;                     /* [r] The file handle that was filled out in the open callback. */
        public uint     offset;                     /* [r] Seek position, make sure you read from this file offset. */
        public uint     sizebytes;                  /* [r] how many bytes requested for read. */
        public int      priority;                   /* [r] 0 = low importance.  100 = extremely important (ie 'must read now or stuttering may occur') */

        public IntPtr   userdata;                   /* [r] User data pointer. */
        public IntPtr   buffer;                     /* [w] Buffer to read file data into. */
        public uint     bytesread;                  /* [w] Fill this in before setting result code to tell FMOD how many bytes were read. */
        public FILE_ASYNCDONE_FUNC done;            /* [r] FMOD file system wake up function.  Call this when user file read is finished.  Pass result of file read as a parameter. */
    }

    public enum OUTPUTTYPE : int
    {
        AUTODETECT,      /* Picks the best output mode for the platform. This is the default. */

        UNKNOWN,         /* All - 3rd party plugin, unknown. This is for use with System::getOutput only. */
        NOSOUND,         /* All - Perform all mixing but discard the final output. */
        WAVWRITER,       /* All - Writes output to a .wav file. */
        NOSOUND_NRT,     /* All - Non-realtime version of FMOD_OUTPUTTYPE_NOSOUND. User can drive mixer with System::update at whatever rate they want. */
        WAVWRITER_NRT,   /* All - Non-realtime version of FMOD_OUTPUTTYPE_WAVWRITER. User can drive mixer with System::update at whatever rate they want. */

        WASAPI,          /* Win / Xbox One       - Windows Audio Session API.           (Default on Windows and Xbox One) */
        ASIO,            /* Win                  - Low latency ASIO 2.0. */
        PULSEAUDIO,      /* Linux                - Pulse Audio.                         (Default on Linux if available) */
        ALSA,            /* Linux                - Advanced Linux Sound Architecture.   (Default on Linux if PulseAudio isn't available) */
        COREAUDIO,       /* Mac / iOS            - Core Audio.                          (Default on Mac and iOS) */
        AUDIOTRACK,      /* Android              - Java Audio Track.                    (Default on Android 2.2 and below) */
        OPENSL,          /* Android              - OpenSL ES.                           (Default on Android 2.3 up to 7.1) */
        AUDIOOUT,        /* PS4                  - Audio Out.                           (Default on PS4) */
        AUDIO3D,         /* PS4                  - Audio3D. */
        WEBAUDIO,        /* Web Browser          - JavaScript webaudio output.          (Default on JavaScript) */
        NNAUDIO,         /* Switch               - nn::audio.                           (Default on Switch) */
        WINSONIC,        /* Win10 / Xbox One     - Windows Sonic. */
        AAUDIO,          /* Android              - AAudio                               (Default on Android 8.0 and above) */

        MAX,             /* Maximum number of output types supported. */
    }

    public enum DEBUG_MODE : int
    {
        TTY,        /* Default log location per platform, i.e. Visual Studio output window, stderr, LogCat, etc */
        FILE,       /* Write log to specified file path */
        CALLBACK,   /* Call specified callback with log information */
    }

    [Flags]
    public enum DEBUG_FLAGS : uint
    {
        NONE                    = 0x00000000,   /* Disable all messages */
        ERROR                   = 0x00000001,   /* Enable only error messages. */
        WARNING                 = 0x00000002,   /* Enable warning and error messages. */
        LOG                     = 0x00000004,   /* Enable informational, warning and error messages (default). */

        TYPE_MEMORY             = 0x00000100,   /* Verbose logging for memory operations, only use this if you are debugging a memory related issue. */
        TYPE_FILE               = 0x00000200,   /* Verbose logging for file access, only use this if you are debugging a file related issue. */
        TYPE_CODEC              = 0x00000400,   /* Verbose logging for codec initialization, only use this if you are debugging a codec related issue. */
        TYPE_TRACE              = 0x00000800,   /* Verbose logging for internal errors, use this for tracking the origin of error codes. */

        DISPLAY_TIMESTAMPS      = 0x00010000,   /* Display the time stamp of the log message in milliseconds. */
        DISPLAY_LINENUMBERS     = 0x00020000,   /* Display the source code file and line number for where the message originated. */
        DISPLAY_THREAD          = 0x00040000,   /* Display the thread ID of the calling function that generated the message. */
    }

    [Flags]
    public enum MEMORY_TYPE : uint
    {
        NORMAL             = 0x00000000,       /* Standard memory. */
        STREAM_FILE        = 0x00000001,       /* Stream file buffer, size controllable with System::setStreamBufferSize. */
        STREAM_DECODE      = 0x00000002,       /* Stream decode buffer, size controllable with FMOD_CREATESOUNDEXINFO::decodebuffersize. */
        SAMPLEDATA         = 0x00000004,       /* Sample data buffer.  Raw audio data, usually PCM/MPEG/ADPCM/XMA data. */
        DSP_BUFFER         = 0x00000008,       /* DSP memory block allocated when more than 1 output exists on a DSP node. */
        PLUGIN             = 0x00000010,       /* Memory allocated by a third party plugin. */
        PERSISTENT         = 0x00200000,       /* Persistent memory. Memory will be freed when System::release is called. */
        ALL                = 0xFFFFFFFF
    }

    public enum SPEAKERMODE : int
    {
        DEFAULT,          /* Default speaker mode for the chosen output mode which will resolve after System::init. */
        RAW,              /* Assume there is no special mapping from a given channel to a speaker, channels map 1:1 in order. Use System::setSoftwareFormat to specify the speaker count. */
        MONO,             /*  1 speaker setup (monaural). */
        STEREO,           /*  2 speaker setup (stereo) front left, front right. */
        QUAD,             /*  4 speaker setup (4.0)    front left, front right, surround left, surround right. */
        SURROUND,         /*  5 speaker setup (5.0)    front left, front right, center, surround left, surround right. */
        _5POINT1,         /*  6 speaker setup (5.1)    front left, front right, center, low frequency, surround left, surround right. */
        _7POINT1,         /*  8 speaker setup (7.1)    front left, front right, center, low frequency, surround left, surround right, back left, back right. */
        _7POINT1POINT4,   /* 12 speaker setup (7.1.4)  front left, front right, center, low frequency, surround left, surround right, back left, back right, top front left, top front right, top back left, top back right. */

        MAX,              /* Maximum number of speaker modes supported. */
    }
     
    public enum SPEAKER : int
    {
        NONE = -1,         /* No speaker */
        FRONT_LEFT,        /* The front left speaker */
        FRONT_RIGHT,       /* The front right speaker */
        FRONT_CENTER,      /* The front center speaker */
        LOW_FREQUENCY,     /* The LFE or 'subwoofer' speaker */
        SURROUND_LEFT,     /* The surround left (usually to the side) speaker */
        SURROUND_RIGHT,    /* The surround right (usually to the side) speaker */
        BACK_LEFT,         /* The back left speaker */
        BACK_RIGHT,        /* The back right speaker */
        TOP_FRONT_LEFT,    /* The top front left speaker */
        TOP_FRONT_RIGHT,   /* The top front right speaker */
        TOP_BACK_LEFT,     /* The top back left speaker */
        TOP_BACK_RIGHT,    /* The top back right speaker */

        MAX,               /* Maximum number of speaker types supported. */
    }

    [Flags]
    public enum CHANNELMASK : uint
    {
        FRONT_LEFT             = 0x00000001,
        FRONT_RIGHT            = 0x00000002,
        FRONT_CENTER           = 0x00000004,
        LOW_FREQUENCY          = 0x00000008,
        SURROUND_LEFT          = 0x00000010,
        SURROUND_RIGHT         = 0x00000020,
        BACK_LEFT              = 0x00000040,
        BACK_RIGHT             = 0x00000080,
        BACK_CENTER            = 0x00000100,

        MONO                   = (FRONT_LEFT),
        STEREO                 = (FRONT_LEFT | FRONT_RIGHT),
        LRC                    = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER),
        QUAD                   = (FRONT_LEFT | FRONT_RIGHT | SURROUND_LEFT | SURROUND_RIGHT),
        SURROUND               = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | SURROUND_LEFT | SURROUND_RIGHT),
        _5POINT1               = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | LOW_FREQUENCY | SURROUND_LEFT | SURROUND_RIGHT),
        _5POINT1_REARS         = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | LOW_FREQUENCY | BACK_LEFT | BACK_RIGHT),
        _7POINT0               = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | SURROUND_LEFT | SURROUND_RIGHT | BACK_LEFT | BACK_RIGHT),
        _7POINT1               = (FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | LOW_FREQUENCY | SURROUND_LEFT | SURROUND_RIGHT | BACK_LEFT | BACK_RIGHT)
    }

    public enum CHANNELORDER : int
    {
        DEFAULT,              /* Left, Right, Center, LFE, Surround Left, Surround Right, Back Left, Back Right (see FMOD_SPEAKER enumeration)   */
        WAVEFORMAT,           /* Left, Right, Center, LFE, Back Left, Back Right, Surround Left, Surround Right (as per Microsoft .wav WAVEFORMAT structure master order) */
        PROTOOLS,             /* Left, Center, Right, Surround Left, Surround Right, LFE */
        ALLMONO,              /* Mono, Mono, Mono, Mono, Mono, Mono, ... (each channel all the way up to 32 channels are treated as if they were mono) */
        ALLSTEREO,            /* Left, Right, Left, Right, Left, Right, ... (each pair of channels is treated as stereo all the way up to 32 channels) */
        ALSA,                 /* Left, Right, Surround Left, Surround Right, Center, LFE (as per Linux ALSA channel order) */

        MAX,                  /* Maximum number of channel orderings supported. */
    }

    public enum PLUGINTYPE : int
    {
        OUTPUT,          /* The plugin type is an output module.  FMOD mixed audio will play through one of these devices */
        CODEC,           /* The plugin type is a file format codec.  FMOD will use these codecs to load file formats for playback. */
        DSP,             /* The plugin type is a DSP unit.  FMOD will use these plugins as part of its DSP network to apply effects to output or generate sound in realtime. */

        MAX,             /* Maximum number of plugin types supported. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PLUGINLIST
    {
        PLUGINTYPE type;
        IntPtr description;
    }

    [Flags]
    public enum INITFLAGS : uint
    {
        NORMAL                     = 0x00000000, /* Initialize normally */
        STREAM_FROM_UPDATE         = 0x00000001, /* No stream thread is created internally.  Streams are driven from System::update.  Mainly used with non-realtime outputs. */
        MIX_FROM_UPDATE            = 0x00000002, /* FMOD Mixer thread is woken up to do a mix when System::update is called rather than waking periodically on its own timer. */
        _3D_RIGHTHANDED            = 0x00000004, /* FMOD will treat +X as right, +Y as up and +Z as backwards (towards you). */
        CHANNEL_LOWPASS            = 0x00000100, /* All FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which is automatically used when Channel::set3DOcclusion is used or the geometry API.   This also causes sounds to sound duller when the sound goes behind the listener, as a fake HRTF style effect.  Use System::setAdvancedSettings to disable or adjust cutoff frequency for this feature. */
        CHANNEL_DISTANCEFILTER     = 0x00000200, /* All FMOD_3D based voices will add a software lowpass and highpass filter effect into the DSP chain which will act as a distance-automated bandpass filter. Use System::setAdvancedSettings to adjust the center frequency. */
        PROFILE_ENABLE             = 0x00010000, /* Enable TCP/IP based host which allows FMOD Designer or FMOD Profiler to connect to it, and view memory, CPU and the DSP network graph in real-time. */
        VOL0_BECOMES_VIRTUAL       = 0x00020000, /* Any sounds that are 0 volume will go virtual and not be processed except for having their positions updated virtually.  Use System::setAdvancedSettings to adjust what volume besides zero to switch to virtual at. */
        GEOMETRY_USECLOSEST        = 0x00040000, /* With the geometry engine, only process the closest polygon rather than accumulating all polygons the sound to listener line intersects. */
        PREFER_DOLBY_DOWNMIX       = 0x00080000, /* When using FMOD_SPEAKERMODE_5POINT1 with a stereo output device, use the Dolby Pro Logic II downmix algorithm instead of the default stereo downmix algorithm. */
        THREAD_UNSAFE              = 0x00100000, /* Disables thread safety for API calls. Only use this if FMOD low level is being called from a single thread, and if Studio API is not being used! */
        PROFILE_METER_ALL          = 0x00200000, /* Slower, but adds level metering for every single DSP unit in the graph.  Use DSP::setMeteringEnabled to turn meters off individually. */
    }

    public enum SOUND_TYPE : int
    {
        UNKNOWN,         /* 3rd party / unknown plugin format. */
        AIFF,            /* AIFF. */
        ASF,             /* Microsoft Advanced Systems Format (ie WMA/ASF/WMV). */
        DLS,             /* Sound font / downloadable sound bank. */
        FLAC,            /* FLAC lossless codec. */
        FSB,             /* FMOD Sample Bank. */
        IT,              /* Impulse Tracker. */
        MIDI,            /* MIDI. extracodecdata is a pointer to an FMOD_MIDI_EXTRACODECDATA structure. */
        MOD,             /* Protracker / Fasttracker MOD. */
        MPEG,            /* MP2/MP3 MPEG. */
        OGGVORBIS,       /* Ogg vorbis. */
        PLAYLIST,        /* Information only from ASX/PLS/M3U/WAX playlists */
        RAW,             /* Raw PCM data. */
        S3M,             /* ScreamTracker 3. */
        USER,            /* User created sound. */
        WAV,             /* Microsoft WAV. */
        XM,              /* FastTracker 2 XM. */
        XMA,             /* Xbox One XMA */
        AUDIOQUEUE,      /* iPhone hardware decoder, supports AAC, ALAC and MP3. extracodecdata is a pointer to an FMOD_AUDIOQUEUE_EXTRACODECDATA structure. */
        AT9,             /* PS4 ATRAC 9 format */
        VORBIS,          /* Vorbis */
        MEDIA_FOUNDATION,/* Windows Store Application built in system codecs */
        MEDIACODEC,      /* Android MediaCodec */
        FADPCM,          /* FMOD Adaptive Differential Pulse Code Modulation */
        OPUS,            /* Opus */

        MAX,             /* Maximum number of sound types supported. */
    }

    public enum SOUND_FORMAT : int
    {
        NONE,       /* Unitialized / unknown */
        PCM8,       /* 8bit integer PCM data */
        PCM16,      /* 16bit integer PCM data  */
        PCM24,      /* 24bit integer PCM data  */
        PCM32,      /* 32bit integer PCM data  */
        PCMFLOAT,   /* 32bit floating point PCM data  */
        BITSTREAM,  /* Sound data is in its native compressed format. */

        MAX         /* Maximum number of sound formats supported. */
    }

    [Flags]
    public enum MODE : uint
    {
        DEFAULT                = 0x00000000,    /* Default for all modes listed below. FMOD_LOOP_OFF, FMOD_2D, FMOD_3D_WORLDRELATIVE, FMOD_3D_INVERSEROLLOFF */
        LOOP_OFF               = 0x00000001,    /* For non looping sounds. (default).  Overrides FMOD_LOOP_NORMAL / FMOD_LOOP_BIDI. */
        LOOP_NORMAL            = 0x00000002,    /* For forward looping sounds. */
        LOOP_BIDI              = 0x00000004,    /* For bidirectional looping sounds. (only works on software mixed static sounds). */
        _2D                    = 0x00000008,    /* Ignores any 3d processing. (default). */
        _3D                    = 0x00000010,    /* Makes the sound positionable in 3D.  Overrides FMOD_2D. */
        CREATESTREAM           = 0x00000080,    /* Decompress at runtime, streaming from the source provided (standard stream).  Overrides FMOD_CREATESAMPLE. */
        CREATESAMPLE           = 0x00000100,    /* Decompress at loadtime, decompressing or decoding whole file into memory as the target sample format. (standard sample). */
        CREATECOMPRESSEDSAMPLE = 0x00000200,    /* Load MP2, MP3, IMAADPCM or XMA into memory and leave it compressed.  During playback the FMOD software mixer will decode it in realtime as a 'compressed sample'.  Can only be used in combination with FMOD_SOFTWARE. */
        OPENUSER               = 0x00000400,    /* Opens a user created static sample or stream. Use FMOD_CREATESOUNDEXINFO to specify format and/or read callbacks.  If a user created 'sample' is created with no read callback, the sample will be empty.  Use FMOD_Sound_Lock and FMOD_Sound_Unlock to place sound data into the sound if this is the case. */
        OPENMEMORY             = 0x00000800,    /* "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds. */
        OPENMEMORY_POINT       = 0x10000000,    /* "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.  Use FMOD_CREATESOUNDEXINFO to specify length.  This differs to FMOD_OPENMEMORY in that it uses the memory as is, without duplicating the memory into its own buffers.  Cannot be freed after open, only after Sound::release.   Will not work if the data is compressed and FMOD_CREATECOMPRESSEDSAMPLE is not used. */
        OPENRAW                = 0x00001000,    /* Will ignore file format and treat as raw pcm.  User may need to declare if data is FMOD_SIGNED or FMOD_UNSIGNED */
        OPENONLY               = 0x00002000,    /* Just open the file, dont prebuffer or read.  Good for fast opens for info, or when sound::readData is to be used. */
        ACCURATETIME           = 0x00004000,    /* For FMOD_CreateSound - for accurate FMOD_Sound_GetLength / FMOD_Channel_SetPosition on VBR MP3, AAC and MOD/S3M/XM/IT/MIDI files.  Scans file first, so takes longer to open. FMOD_OPENONLY does not affect this. */
        MPEGSEARCH             = 0x00008000,    /* For corrupted / bad MP3 files.  This will search all the way through the file until it hits a valid MPEG header.  Normally only searches for 4k. */
        NONBLOCKING            = 0x00010000,    /* For opening sounds and getting streamed subsounds (seeking) asyncronously.  Use Sound::getOpenState to poll the state of the sound as it opens or retrieves the subsound in the background. */
        UNIQUE                 = 0x00020000,    /* Unique sound, can only be played one at a time */
        _3D_HEADRELATIVE       = 0x00040000,    /* Make the sound's position, velocity and orientation relative to the listener. */
        _3D_WORLDRELATIVE      = 0x00080000,    /* Make the sound's position, velocity and orientation absolute (relative to the world). (DEFAULT) */
        _3D_INVERSEROLLOFF     = 0x00100000,    /* This sound will follow the inverse rolloff model where mindistance = full volume, maxdistance = where sound stops attenuating, and rolloff is fixed according to the global rolloff factor.  (DEFAULT) */
        _3D_LINEARROLLOFF      = 0x00200000,    /* This sound will follow a linear rolloff model where mindistance = full volume, maxdistance = silence.  */
        _3D_LINEARSQUAREROLLOFF= 0x00400000,    /* This sound will follow a linear-square rolloff model where mindistance = full volume, maxdistance = silence.  Rolloffscale is ignored. */
        _3D_INVERSETAPEREDROLLOFF = 0x00800000, /* This sound will follow the inverse rolloff model at distances close to mindistance and a linear-square rolloff close to maxdistance. */
        _3D_CUSTOMROLLOFF      = 0x04000000,    /* This sound will follow a rolloff model defined by Sound::set3DCustomRolloff / Channel::set3DCustomRolloff.  */
        _3D_IGNOREGEOMETRY     = 0x40000000,    /* Is not affect by geometry occlusion.  If not specified in Sound::setMode, or Channel::setMode, the flag is cleared and it is affected by geometry again. */
        IGNORETAGS             = 0x02000000,    /* Skips id3v2/asf/etc tag checks when opening a sound, to reduce seek/read overhead when opening files (helps with CD performance). */
        LOWMEM                 = 0x08000000,    /* Removes some features from samples to give a lower memory overhead, like Sound::getName. */
        VIRTUAL_PLAYFROMSTART  = 0x80000000     /* For sounds that start virtual (due to being quiet or low importance), instead of swapping back to audible, and playing at the correct offset according to time, this flag makes the sound play from the start. */
    }

    public enum OPENSTATE : int
    {
        READY = 0,       /* Opened and ready to play */
        LOADING,         /* Initial load in progress */
        ERROR,           /* Failed to open - file not found, out of memory etc.  See return value of Sound::getOpenState for what happened. */
        CONNECTING,      /* Connecting to remote host (internet sounds only) */
        BUFFERING,       /* Buffering data */
        SEEKING,         /* Seeking to subsound and re-flushing stream buffer. */
        PLAYING,         /* Ready and playing, but not possible to release at this time without stalling the main thread. */
        SETPOSITION,     /* Seeking within a stream to a different position. */

        MAX,             /* Maximum number of open state types. */
    }

    public enum SOUNDGROUP_BEHAVIOR : int
    {
        BEHAVIOR_FAIL,              /* Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will simply fail during System::playSound. */
        BEHAVIOR_MUTE,              /* Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will be silent, then if another sound in the group stops the sound that was silent before becomes audible again. */
        BEHAVIOR_STEALLOWEST,       /* Any sound played that puts the sound count over the SoundGroup::setMaxAudible setting, will steal the quietest / least important sound playing in the group. */

        MAX,                        /* Maximum number of sound group behaviors. */
    }

    public enum CHANNELCONTROL_CALLBACK_TYPE : int
    {
        END,                  /* Called when a sound ends. */
        VIRTUALVOICE,         /* Called when a voice is swapped out or swapped in. */
        SYNCPOINT,            /* Called when a syncpoint is encountered.  Can be from wav file markers. */
        OCCLUSION,            /* Called when the channel has its geometry occlusion value calculated.  Can be used to clamp or change the value. */

        MAX,                  /* Maximum number of callback types supported. */
    }

    public struct CHANNELCONTROL_DSP_INDEX
    {
        public const int HEAD    = -1;         /* Head of the DSP chain.   Equivalent of index 0. */
        public const int FADER   = -2;         /* Built in fader DSP. */
        public const int TAIL    = -3;         /* Tail of the DSP chain.  Equivalent of the number of dsps minus 1. */
    }

    public enum ERRORCALLBACK_INSTANCETYPE : int
    {
        NONE,
        SYSTEM,
        CHANNEL,
        CHANNELGROUP,
        CHANNELCONTROL,
        SOUND,
        SOUNDGROUP,
        DSP,
        DSPCONNECTION,
        GEOMETRY,
        REVERB3D,
        STUDIO_SYSTEM,
        STUDIO_EVENTDESCRIPTION,
        STUDIO_EVENTINSTANCE,
        STUDIO_PARAMETERINSTANCE,
        STUDIO_BUS,
        STUDIO_VCA,
        STUDIO_BANK,
        STUDIO_COMMANDREPLAY
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ERRORCALLBACK_INFO
    {
        public  RESULT                      result;                     /* Error code result */
        public  ERRORCALLBACK_INSTANCETYPE  instancetype;               /* Type of instance the error occurred on */
        public  IntPtr                      instance;                   /* Instance pointer */
        public  StringWrapper               functionname;               /* Function that the error occurred on */
        public  StringWrapper               functionparams;             /* Function parameters that the error ocurred on */
    }

    [Flags]
    public enum SYSTEM_CALLBACK_TYPE : uint
    {
        DEVICELISTCHANGED      = 0x00000001,  /* Called from System::update when the enumerated list of devices has changed. */
        DEVICELOST             = 0x00000002,  /* Called from System::update when an output device has been lost due to control panel parameter changes and FMOD cannot automatically recover. */
        MEMORYALLOCATIONFAILED = 0x00000004,  /* Called directly when a memory allocation fails somewhere in FMOD.  (NOTE - 'system' will be NULL in this callback type.)*/
        THREADCREATED          = 0x00000008,  /* Called directly when a thread is created. (NOTE - 'system' will be NULL in this callback type.) */
        BADDSPCONNECTION       = 0x00000010,  /* Called when a bad connection was made with DSP::addInput. Usually called from mixer thread because that is where the connections are made.  */
        PREMIX                 = 0x00000020,  /* Called each tick before a mix update happens. */
        POSTMIX                = 0x00000040,  /* Called each tick after a mix update happens. */
        ERROR                  = 0x00000080,  /* Called when each API function returns an error code, including delayed async functions. */
        MIDMIX                 = 0x00000100,  /* Called each tick in mix update after clocks have been updated before the main mix occurs. */
        THREADDESTROYED        = 0x00000200,  /* Called directly when a thread is destroyed. */
        PREUPDATE              = 0x00000400,  /* Called at start of System::update function. */
        POSTUPDATE             = 0x00000800,  /* Called at end of System::update function. */
        RECORDLISTCHANGED      = 0x00001000,  /* Called from System::update when the enumerated list of recording devices has changed. */
        ALL                    = 0xFFFFFFFF,  /* Pass this mask to System::setCallback to receive all callback types.  */
    }

    /*
        FMOD Callbacks
    */
    public delegate RESULT DEBUG_CALLBACK           (DEBUG_FLAGS flags, StringWrapper file, int line, StringWrapper func, StringWrapper message);
    public delegate RESULT SYSTEM_CALLBACK          (IntPtr system, SYSTEM_CALLBACK_TYPE type, IntPtr commanddata1, IntPtr commanddata2, IntPtr userdata);
    public delegate RESULT CHANNELCONTROL_CALLBACK(IntPtr channelcontrol, CHANNELCONTROL_TYPE controltype, CHANNELCONTROL_CALLBACK_TYPE callbacktype, IntPtr commanddata1, IntPtr commanddata2);
    public delegate RESULT SOUND_NONBLOCK_CALLBACK  (IntPtr sound, RESULT result);
    public delegate RESULT SOUND_PCMREAD_CALLBACK   (IntPtr sound, IntPtr data, uint datalen);
    public delegate RESULT SOUND_PCMSETPOS_CALLBACK (IntPtr sound, int subsound, uint position, TIMEUNIT postype);
    public delegate RESULT FILE_OPEN_CALLBACK       (StringWrapper name, ref uint filesize, ref IntPtr handle, IntPtr userdata);
    public delegate RESULT FILE_CLOSE_CALLBACK      (IntPtr handle, IntPtr userdata);
    public delegate RESULT FILE_READ_CALLBACK       (IntPtr handle, IntPtr buffer, uint sizebytes, ref uint bytesread, IntPtr userdata);
    public delegate RESULT FILE_SEEK_CALLBACK       (IntPtr handle, uint pos, IntPtr userdata);
    public delegate RESULT FILE_ASYNCREAD_CALLBACK  (IntPtr info, IntPtr userdata);
    public delegate RESULT FILE_ASYNCCANCEL_CALLBACK(IntPtr info, IntPtr userdata);
    public delegate RESULT FILE_ASYNCDONE_FUNC      (IntPtr info, RESULT result);
    public delegate IntPtr MEMORY_ALLOC_CALLBACK    (uint size, MEMORY_TYPE type, StringWrapper sourcestr);
    public delegate IntPtr MEMORY_REALLOC_CALLBACK  (IntPtr ptr, uint size, MEMORY_TYPE type, StringWrapper sourcestr);
    public delegate void   MEMORY_FREE_CALLBACK     (IntPtr ptr, MEMORY_TYPE type, StringWrapper sourcestr);
    public delegate float  CB_3D_ROLLOFF_CALLBACK      (IntPtr channelcontrol, float distance);

    public enum DSP_RESAMPLER : int
    {
        DEFAULT,         /* Default interpolation method.  Currently equal to FMOD_DSP_RESAMPLER_LINEAR. */
        NOINTERP,        /* No interpolation.  High frequency aliasing hiss will be audible depending on the sample rate of the sound. */
        LINEAR,          /* Linear interpolation (default method).  Fast and good quality, causes very slight lowpass effect on low frequency sounds. */
        CUBIC,           /* Cubic interpolation.  Slower than linear interpolation but better quality. */
        SPLINE,          /* 5 point spline interpolation.  Slowest resampling method but best quality. */

        MAX,             /* Maximum number of resample methods supported. */
    }

    public enum DSPCONNECTION_TYPE : int
    {
        STANDARD,          /* Default connection type.         Audio is mixed from the input to the output DSP's audible buffer.  */
        SIDECHAIN,         /* Sidechain connection type.       Audio is mixed from the input to the output DSP's sidechain buffer.  */
        SEND,              /* Send connection type.            Audio is mixed from the input to the output DSP's audible buffer, but the input is NOT executed, only copied from.  A standard connection or sidechain needs to make an input execute to generate data. */
        SEND_SIDECHAIN,    /* Send sidechain connection type.  Audio is mixed from the input to the output DSP's sidechain buffer, but the input is NOT executed, only copied from.  A standard connection or sidechain needs to make an input execute to generate data. */

        MAX,               /* Maximum number of DSP connection types supported. */
    }

    public enum TAGTYPE : int
    {
        UNKNOWN = 0,
        ID3V1,
        ID3V2,
        VORBISCOMMENT,
        SHOUTCAST,
        ICECAST,
        ASF,
        MIDI,
        PLAYLIST,
        FMOD,
        USER,

        MAX                /* Maximum number of tag types supported. */
    }

    public enum TAGDATATYPE : int
    {
        BINARY = 0,
        INT,
        FLOAT,
        STRING,
        STRING_UTF16,
        STRING_UTF16BE,
        STRING_UTF8,

        MAX                /* Maximum number of tag datatypes supported. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TAG
    {
        public  TAGTYPE           type;         /* [r] The type of this tag. */
        public  TAGDATATYPE       datatype;     /* [r] The type of data that this tag contains */
        public  StringWrapper     name;         /* [r] The name of this tag i.e. "TITLE", "ARTIST" etc. */
        public  IntPtr            data;         /* [r] Pointer to the tag data - its format is determined by the datatype member */
        public  uint              datalen;      /* [r] Length of the data contained in this tag */
        public  bool              updated;      /* [r] True if this tag has been updated since last being accessed with Sound::getTag */
    }

    [Flags]
    public enum TIMEUNIT : uint
    {
        MS          = 0x00000001,  /* Milliseconds. */
        PCM         = 0x00000002,  /* PCM Samples, related to milliseconds * samplerate / 1000. */
        PCMBYTES    = 0x00000004,  /* Bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes). */
        RAWBYTES    = 0x00000008,  /* Raw file bytes of (compressed) sound data (does not include headers).  Only used by Sound::getLength and Channel::getPosition. */
        PCMFRACTION = 0x00000010,  /* Fractions of 1 PCM sample.  Unsigned int range 0 to 0xFFFFFFFF.  Used for sub-sample granularity for DSP purposes. */
        MODORDER    = 0x00000100,  /* MOD/S3M/XM/IT.  Order in a sequenced module format.  Use Sound::getFormat to determine the format. */
        MODROW      = 0x00000200,  /* MOD/S3M/XM/IT.  Current row in a sequenced module format.  Cannot use with Channel::setPosition.  Sound::getLength will return the number if rows in the currently playing or seeked to pattern. */
        MODPATTERN  = 0x00000400,  /* MOD/S3M/XM/IT.  Current pattern in a sequenced module format.  Cannot use with Channel::setPosition.  Sound::getLength will return the number of patterns in the song and Channel::getPosition will return the currently playing pattern. */
    }

    public struct PORT_INDEX
    {
        public const ulong NONE = 0xFFFFFFFFFFFFFFFF;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CREATESOUNDEXINFO
    {
        public int                         cbsize;                 /* [w]   Size of this structure.  This is used so the structure can be expanded in the future and still work on older versions of FMOD Ex. */
        public uint                        length;                 /* [w]   Optional. Specify 0 to ignore. Size in bytes of file to load, or sound to create (in this case only if FMOD_OPENUSER is used).  Required if loading from memory.  If 0 is specified, then it will use the size of the file (unless loading from memory then an error will be returned). */
        public uint                        fileoffset;             /* [w]   Optional. Specify 0 to ignore. Offset from start of the file to start loading from.  This is useful for loading files from inside big data files. */
        public int                         numchannels;            /* [w]   Optional. Specify 0 to ignore. Number of channels in a sound specified only if OPENUSER is used. */
        public int                         defaultfrequency;       /* [w]   Optional. Specify 0 to ignore. Default frequency of sound in a sound specified only if OPENUSER is used.  Other formats use the frequency determined by the file format. */
        public SOUND_FORMAT                format;                 /* [w]   Optional. Specify 0 or SOUND_FORMAT_NONE to ignore. Format of the sound specified only if OPENUSER is used.  Other formats use the format determined by the file format.   */
        public uint                        decodebuffersize;       /* [w]   Optional. Specify 0 to ignore. For streams.  This determines the size of the double buffer (in PCM samples) that a stream uses.  Use this for user created streams if you want to determine the size of the callback buffer passed to you.  Specify 0 to use FMOD's default size which is currently equivalent to 400ms of the sound format created/loaded. */
        public int                         initialsubsound;        /* [w]   Optional. Specify 0 to ignore. In a multi-sample file format such as .FSB/.DLS/.SF2, specify the initial subsound to seek to, only if CREATESTREAM is used. */
        public int                         numsubsounds;           /* [w]   Optional. Specify 0 to ignore or have no subsounds.  In a user created multi-sample sound, specify the number of subsounds within the sound that are accessable with Sound::getSubSound / SoundGetSubSound. */
        public IntPtr                      inclusionlist;          /* [w]   Optional. Specify 0 to ignore. In a multi-sample format such as .FSB/.DLS/.SF2 it may be desirable to specify only a subset of sounds to be loaded out of the whole file.  This is an array of subsound indicies to load into memory when created. */
        public int                         inclusionlistnum;       /* [w]   Optional. Specify 0 to ignore. This is the number of integers contained within the */
        public SOUND_PCMREAD_CALLBACK      pcmreadcallback;        /* [w]   Optional. Specify 0 to ignore. Callback to 'piggyback' on FMOD's read functions and accept or even write PCM data while FMOD is opening the sound.  Used for user sounds created with OPENUSER or for capturing decoded data as FMOD reads it. */
        public SOUND_PCMSETPOS_CALLBACK    pcmsetposcallback;      /* [w]   Optional. Specify 0 to ignore. Callback for when the user calls a seeking function such as Channel::setPosition within a multi-sample sound, and for when it is opened.*/
        public SOUND_NONBLOCK_CALLBACK     nonblockcallback;       /* [w]   Optional. Specify 0 to ignore. Callback for successful completion, or error while loading a sound that used the FMOD_NONBLOCKING flag.*/
        public IntPtr                      dlsname;                /* [w]   Optional. Specify 0 to ignore. Filename for a DLS or SF2 sample set when loading a MIDI file.   If not specified, on windows it will attempt to open /windows/system32/drivers/gm.dls, otherwise the MIDI will fail to open.  */
        public IntPtr                      encryptionkey;          /* [w]   Optional. Specify 0 to ignore. Key for encrypted FSB file.  Without this key an encrypted FSB file will not load. */
        public int                         maxpolyphony;           /* [w]   Optional. Specify 0 to ingore. For sequenced formats with dynamic channel allocation such as .MID and .IT, this specifies the maximum voice count allowed while playing.  .IT defaults to 64.  .MID defaults to 32. */
        public IntPtr                      userdata;               /* [w]   Optional. Specify 0 to ignore. This is user data to be attached to the sound during creation.  Access via Sound::getUserData. */
        public SOUND_TYPE                  suggestedsoundtype;     /* [w]   Optional. Specify 0 or FMOD_SOUND_TYPE_UNKNOWN to ignore.  Instead of scanning all codec types, use this to speed up loading by making it jump straight to this codec. */
        public FILE_OPEN_CALLBACK          fileuseropen;           /* [w]   Optional. Specify 0 to ignore. Callback for opening this file. */
        public FILE_CLOSE_CALLBACK         fileuserclose;          /* [w]   Optional. Specify 0 to ignore. Callback for closing this file. */
        public FILE_READ_CALLBACK          fileuserread;           /* [w]   Optional. Specify 0 to ignore. Callback for reading from this file. */
        public FILE_SEEK_CALLBACK          fileuserseek;           /* [w]   Optional. Specify 0 to ignore. Callback for seeking within this file. */
        public FILE_ASYNCREAD_CALLBACK     fileuserasyncread;      /* [w]   Optional. Specify 0 to ignore. Callback for asyncronously reading from this file. */
        public FILE_ASYNCCANCEL_CALLBACK   fileuserasynccancel;    /* [w]   Optional. Specify 0 to ignore. Callback for cancelling an asyncronous read. */
        public IntPtr                      fileuserdata;           /* [w]   Optional. Specify 0 to ignore. User data to be passed into the file callbacks. */
        public int                         filebuffersize;         /* [w]   Optional. Specify 0 to ignore. Buffer size for reading the file, -1 to disable buffering, or 0 for system default. */
        public CHANNELORDER                channelorder;           /* [w]   Optional. Specify 0 to ignore. Use this to differ the way fmod maps multichannel sounds to speakers.  See FMOD_CHANNELORDER for more. */
        public IntPtr                      initialsoundgroup;      /* [w]   Optional. Specify 0 to ignore. Specify a sound group if required, to put sound in as it is created. */
        public uint                        initialseekposition;    /* [w]   Optional. Specify 0 to ignore. For streams. Specify an initial position to seek the stream to. */
        public TIMEUNIT                    initialseekpostype;     /* [w]   Optional. Specify 0 to ignore. For streams. Specify the time unit for the position set in initialseekposition. */
        public int                         ignoresetfilesystem;    /* [w]   Optional. Specify 0 to ignore. Set to 1 to use fmod's built in file system. Ignores setFileSystem callbacks and also FMOD_CREATESOUNEXINFO file callbacks.  Useful for specific cases where you don't want to use your own file system but want to use fmod's file system (ie net streaming). */
        public uint                        audioqueuepolicy;       /* [w]   Optional. Specify 0 or FMOD_AUDIOQUEUE_CODECPOLICY_DEFAULT to ignore. Policy used to determine whether hardware or software is used for decoding, see FMOD_AUDIOQUEUE_CODECPOLICY for options (iOS >= 3.0 required, otherwise only hardware is available) */
        public uint                        minmidigranularity;     /* [w]   Optional. Specify 0 to ignore. Allows you to set a minimum desired MIDI mixer granularity. Values smaller than 512 give greater than default accuracy at the cost of more CPU and vise versa. Specify 0 for default (512 samples). */
        public int                         nonblockthreadid;       /* [w]   Optional. Specify 0 to ignore. Specifies a thread index to execute non blocking load on.  Allows for up to 5 threads to be used for loading at once.  This is to avoid one load blocking another.  Maximum value = 4. */
        public IntPtr                      fsbguid;                /* [r/w] Optional. Specify 0 to ignore. Allows you to provide the GUID lookup for cached FSB header info. Once loaded the GUID will be written back to the pointer. This is to avoid seeking and reading the FSB header. */
    }

#pragma warning disable 414
    [StructLayout(LayoutKind.Sequential)]
    public struct REVERB_PROPERTIES
    {                            /*        MIN     MAX    DEFAULT   DESCRIPTION */
        public float DecayTime;         /* [r/w]  0.0    20000.0 1500.0  Reverberation decay time in ms                                        */
        public float EarlyDelay;        /* [r/w]  0.0    300.0   7.0     Initial reflection delay time                                         */
        public float LateDelay;         /* [r/w]  0.0    100     11.0    Late reverberation delay time relative to initial reflection          */
        public float HFReference;       /* [r/w]  20.0   20000.0 5000    Reference high frequency (hz)                                         */
        public float HFDecayRatio;      /* [r/w]  10.0   100.0   50.0    High-frequency to mid-frequency decay time ratio                      */
        public float Diffusion;         /* [r/w]  0.0    100.0   100.0   Value that controls the echo density in the late reverberation decay. */
        public float Density;           /* [r/w]  0.0    100.0   100.0   Value that controls the modal density in the late reverberation decay */
        public float LowShelfFrequency; /* [r/w]  20.0   1000.0  250.0   Reference low frequency (hz)                                          */
        public float LowShelfGain;      /* [r/w]  -36.0  12.0    0.0     Relative room effect level at low frequencies                         */
        public float HighCut;           /* [r/w]  20.0   20000.0 20000.0 Relative room effect level at high frequencies                        */
        public float EarlyLateMix;      /* [r/w]  0.0    100.0   50.0    Early reflections level relative to room effect                       */
        public float WetLevel;          /* [r/w]  -80.0  20.0    -6.0    Room effect level (at mid frequencies)
                                  * */
        #region wrapperinternal
        public REVERB_PROPERTIES(float decayTime, float earlyDelay, float lateDelay, float hfReference,
            float hfDecayRatio, float diffusion, float density, float lowShelfFrequency, float lowShelfGain,
            float highCut, float earlyLateMix, float wetLevel)
        {
            DecayTime = decayTime;
            EarlyDelay = earlyDelay;
            LateDelay = lateDelay;
            HFReference = hfReference;
            HFDecayRatio = hfDecayRatio;
            Diffusion = diffusion;
            Density = density;
            LowShelfFrequency = lowShelfFrequency;
            LowShelfGain = lowShelfGain;
            HighCut = highCut;
            EarlyLateMix = earlyLateMix;
            WetLevel = wetLevel;
        }
        #endregion
    }
#pragma warning restore 414

    public class PRESET
    {
        /*                                                                                  Instance  Env   Diffus  Room   RoomHF  RmLF DecTm   DecHF  DecLF   Refl  RefDel   Revb  RevDel  ModTm  ModDp   HFRef    LFRef   Diffus  Densty  FLAGS */
        public static REVERB_PROPERTIES OFF()                 { return new REVERB_PROPERTIES(  1000,    7,  11, 5000, 100, 100, 100, 250, 0,    20,  96, -80.0f );}
        public static REVERB_PROPERTIES GENERIC()             { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  83, 100, 100, 250, 0, 14500,  96,  -8.0f );}
        public static REVERB_PROPERTIES PADDEDCELL()          { return new REVERB_PROPERTIES(   170,    1,   2, 5000,  10, 100, 100, 250, 0,   160,  84,  -7.8f );}
        public static REVERB_PROPERTIES ROOM()                { return new REVERB_PROPERTIES(   400,    2,   3, 5000,  83, 100, 100, 250, 0,  6050,  88,  -9.4f );}
        public static REVERB_PROPERTIES BATHROOM()            { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  54, 100,  60, 250, 0,  2900,  83,   0.5f );}
        public static REVERB_PROPERTIES LIVINGROOM()          { return new REVERB_PROPERTIES(   500,    3,   4, 5000,  10, 100, 100, 250, 0,   160,  58, -19.0f );}
        public static REVERB_PROPERTIES STONEROOM()           { return new REVERB_PROPERTIES(  2300,   12,  17, 5000,  64, 100, 100, 250, 0,  7800,  71,  -8.5f );}
        public static REVERB_PROPERTIES AUDITORIUM()          { return new REVERB_PROPERTIES(  4300,   20,  30, 5000,  59, 100, 100, 250, 0,  5850,  64, -11.7f );}
        public static REVERB_PROPERTIES CONCERTHALL()         { return new REVERB_PROPERTIES(  3900,   20,  29, 5000,  70, 100, 100, 250, 0,  5650,  80,  -9.8f );}
        public static REVERB_PROPERTIES CAVE()                { return new REVERB_PROPERTIES(  2900,   15,  22, 5000, 100, 100, 100, 250, 0, 20000,  59, -11.3f );}
        public static REVERB_PROPERTIES ARENA()               { return new REVERB_PROPERTIES(  7200,   20,  30, 5000,  33, 100, 100, 250, 0,  4500,  80,  -9.6f );}
        public static REVERB_PROPERTIES HANGAR()              { return new REVERB_PROPERTIES( 10000,   20,  30, 5000,  23, 100, 100, 250, 0,  3400,  72,  -7.4f );}
        public static REVERB_PROPERTIES CARPETTEDHALLWAY()    { return new REVERB_PROPERTIES(   300,    2,  30, 5000,  10, 100, 100, 250, 0,   500,  56, -24.0f );}
        public static REVERB_PROPERTIES HALLWAY()             { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  59, 100, 100, 250, 0,  7800,  87,  -5.5f );}
        public static REVERB_PROPERTIES STONECORRIDOR()       { return new REVERB_PROPERTIES(   270,   13,  20, 5000,  79, 100, 100, 250, 0,  9000,  86,  -6.0f );}
        public static REVERB_PROPERTIES ALLEY()               { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  86, 100, 100, 250, 0,  8300,  80,  -9.8f );}
        public static REVERB_PROPERTIES FOREST()              { return new REVERB_PROPERTIES(  1500,  162,  88, 5000,  54,  79, 100, 250, 0,   760,  94, -12.3f );}
        public static REVERB_PROPERTIES CITY()                { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  67,  50, 100, 250, 0,  4050,  66, -26.0f );}
        public static REVERB_PROPERTIES MOUNTAINS()           { return new REVERB_PROPERTIES(  1500,  300, 100, 5000,  21,  27, 100, 250, 0,  1220,  82, -24.0f );}
        public static REVERB_PROPERTIES QUARRY()              { return new REVERB_PROPERTIES(  1500,   61,  25, 5000,  83, 100, 100, 250, 0,  3400, 100,  -5.0f );}
        public static REVERB_PROPERTIES PLAIN()               { return new REVERB_PROPERTIES(  1500,  179, 100, 5000,  50,  21, 100, 250, 0,  1670,  65, -28.0f );}
        public static REVERB_PROPERTIES PARKINGLOT()          { return new REVERB_PROPERTIES(  1700,    8,  12, 5000, 100, 100, 100, 250, 0, 20000,  56, -19.5f );}
        public static REVERB_PROPERTIES SEWERPIPE()           { return new REVERB_PROPERTIES(  2800,   14,  21, 5000,  14,  80,  60, 250, 0,  3400,  66,   1.2f );}
        public static REVERB_PROPERTIES UNDERWATER()          { return new REVERB_PROPERTIES(  1500,    7,  11, 5000,  10, 100, 100, 250, 0,   500,  92,   7.0f );}
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ADVANCEDSETTINGS
    {
        public int                 cbSize;                     /* [w]   Size of this structure.  Use sizeof(FMOD_ADVANCEDSETTINGS) */
        public int                 maxMPEGCodecs;              /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  MPEG   codecs consume 30,528 bytes per instance and this number will determine how many MPEG   channels can be played simultaneously. Default = 32. */
        public int                 maxADPCMCodecs;             /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  ADPCM  codecs consume  3,128 bytes per instance and this number will determine how many ADPCM  channels can be played simultaneously. Default = 32. */
        public int                 maxXMACodecs;               /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  XMA    codecs consume 14,836 bytes per instance and this number will determine how many XMA    channels can be played simultaneously. Default = 32. */
        public int                 maxVorbisCodecs;            /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  Vorbis codecs consume 23,256 bytes per instance and this number will determine how many Vorbis channels can be played simultaneously. Default = 32. */    
        public int                 maxAT9Codecs;               /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  AT9    codecs consume  8,720 bytes per instance and this number will determine how many AT9    channels can be played simultaneously. Default = 32. */    
        public int                 maxFADPCMCodecs;            /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_CREATECOMPRESSEDSAMPLE only.  This number will determine how many FADPCM channels can be played simultaneously. Default = 32. */
        public int                 maxPCMCodecs;               /* [r/w] Unsupported. */
        public int                 ASIONumChannels;            /* [r/w] Optional. Specify 0 to ignore. Number of channels available on the ASIO device. */
        public IntPtr              ASIOChannelList;            /* [r/w] Optional. Specify 0 to ignore. Pointer to an array of strings (number of entries defined by ASIONumChannels) with ASIO channel names. */
        public IntPtr              ASIOSpeakerList;            /* [r/w] Optional. Specify 0 to ignore. Pointer to a list of speakers that the ASIO channels map to.  This can be called after System::init to remap ASIO output. */
        public float               vol0virtualvol;             /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_INIT_VOL0_BECOMES_VIRTUAL.  If this flag is used, and the volume is below this, then the sound will become virtual.  Use this value to raise the threshold to a different point where a sound goes virtual. */
        public uint                defaultDecodeBufferSize;    /* [r/w] Optional. Specify 0 to ignore. For streams. This determines the default size of the double buffer (in milliseconds) that a stream uses.  Default = 400ms */
        public ushort              profilePort;                /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_INIT_PROFILE_ENABLE.  Specify the port to listen on for connections by the profiler application. */
        public uint                geometryMaxFadeTime;        /* [r/w] Optional. Specify 0 to ignore. The maximum time in miliseconds it takes for a channel to fade to the new level when its occlusion changes. */
        public float               distanceFilterCenterFreq;   /* [r/w] Optional. Specify 0 to ignore. For use with FMOD_INIT_DISTANCE_FILTERING.  The default center frequency in Hz for the distance filtering effect. Default = 1500.0. */
        public int                 reverb3Dinstance;           /* [r/w] Optional. Specify 0 to ignore. Out of 0 to 3, 3d reverb spheres will create a phyical reverb unit on this instance slot.  See FMOD_REVERB_PROPERTIES. */
        public int                 DSPBufferPoolSize;          /* [r/w] Optional. Specify 0 to ignore. Number of buffers in DSP buffer pool.  Each buffer will be DSPBlockSize * sizeof(float) * SpeakerModeChannelCount.  ie 7.1 @ 1024 DSP block size = 8 * 1024 * 4 = 32kb.  Default = 8. */
        public uint                stackSizeStream;            /* [r/w] Optional. Specify 0 to ignore. Specify the stack size for the FMOD Stream thread in bytes.  Useful for custom codecs that use excess stack.  Default 49,152 (48kb) */
        public uint                stackSizeNonBlocking;       /* [r/w] Optional. Specify 0 to ignore. Specify the stack size for the FMOD_NONBLOCKING loading thread.  Useful for custom codecs that use excess stack.  Default 65,536 (64kb) */
        public uint                stackSizeMixer;             /* [r/w] Optional. Specify 0 to ignore. Specify the stack size for the FMOD mixer thread.  Useful for custom dsps that use excess stack.  Default 49,152 (48kb) */
        public DSP_RESAMPLER       resamplerMethod;            /* [r/w] Optional. Specify 0 to ignore. Resampling method used with fmod's software mixer.  See FMOD_DSP_RESAMPLER for details on methods. */
        public uint                commandQueueSize;           /* [r/w] Optional. Specify 0 to ignore. Specify the command queue size for thread safe processing.  Default 2048 (2kb) */
        public uint                randomSeed;                 /* [r/w] Optional. Specify 0 to ignore. Seed value that FMOD will use to initialize its internal random number generators. */
    }

    [Flags]
    public enum DRIVER_STATE : uint
    {
        CONNECTED = 0x00000001, /* Device is currently plugged in. */
        DEFAULT   = 0x00000002, /* Device is the users preferred choice. */
    }

    /*
        FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see System init/close to get started.
    */
    public struct Factory
    {
        public static RESULT System_Create(out System system)
        {
            return FMOD5_System_Create(out system.handle);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Create(out IntPtr system);

        #endregion
    }

    public struct Memory
    {
        public static RESULT Initialize(IntPtr poolmem, int poollen, MEMORY_ALLOC_CALLBACK useralloc, MEMORY_REALLOC_CALLBACK userrealloc, MEMORY_FREE_CALLBACK userfree, MEMORY_TYPE memtypeflags = MEMORY_TYPE.ALL)
        {
            return FMOD5_Memory_Initialize(poolmem, poollen, useralloc, userrealloc, userfree, memtypeflags);
        }

        public static RESULT GetStats(out int currentalloced, out int maxalloced, bool blocking = true)
        {
            return FMOD5_Memory_GetStats(out currentalloced, out maxalloced, blocking);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Memory_Initialize(IntPtr poolmem, int poollen, MEMORY_ALLOC_CALLBACK useralloc, MEMORY_REALLOC_CALLBACK userrealloc, MEMORY_FREE_CALLBACK userfree, MEMORY_TYPE memtypeflags);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Memory_GetStats  (out int currentalloced, out int maxalloced, bool blocking);

        #endregion
    }

    public struct Debug
    {
        public static RESULT Initialize(DEBUG_FLAGS flags, DEBUG_MODE mode = DEBUG_MODE.TTY, DEBUG_CALLBACK callback = null, string filename = null)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_Debug_Initialize(flags, mode, callback, encoder.byteFromStringUTF8(filename));
            }
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Debug_Initialize(DEBUG_FLAGS flags, DEBUG_MODE mode, DEBUG_CALLBACK callback, byte[] filename);

        #endregion
    }

    /*
        'System' API.
    */
    public struct System
    {
        public RESULT release()
        {
            return FMOD5_System_Release(this.handle);
        }

        // Pre-init functions.
        public RESULT setOutput(OUTPUTTYPE output)
        {
            return FMOD5_System_SetOutput(this.handle, output);
        }
        public RESULT getOutput(out OUTPUTTYPE output)
        {
            return FMOD5_System_GetOutput(this.handle, out output);
        }
        public RESULT getNumDrivers(out int numdrivers)
        {
            return FMOD5_System_GetNumDrivers(this.handle, out numdrivers);
        }
        public RESULT getDriverInfo(int id, out string name, int namelen, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_System_GetDriverInfo(this.handle, id, stringMem, namelen, out guid, out systemrate, out speakermode, out speakermodechannels);
            using (StringHelper.ThreadSafeEncoding encoding = StringHelper.GetFreeHelper())
            {
                name = encoding.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getDriverInfo(int id, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels)
        {
            return FMOD5_System_GetDriverInfo(this.handle, id, IntPtr.Zero, 0, out guid, out systemrate, out speakermode, out speakermodechannels);
        }
        public RESULT setDriver(int driver)
        {
            return FMOD5_System_SetDriver(this.handle, driver);
        }
        public RESULT getDriver(out int driver)
        {
            return FMOD5_System_GetDriver(this.handle, out driver);
        }
        public RESULT setSoftwareChannels(int numsoftwarechannels)
        {
            return FMOD5_System_SetSoftwareChannels(this.handle, numsoftwarechannels);
        }
        public RESULT getSoftwareChannels(out int numsoftwarechannels)
        {
            return FMOD5_System_GetSoftwareChannels(this.handle, out numsoftwarechannels);
        }
        public RESULT setSoftwareFormat(int samplerate, SPEAKERMODE speakermode, int numrawspeakers)
        {
            return FMOD5_System_SetSoftwareFormat(this.handle, samplerate, speakermode, numrawspeakers);
        }
        public RESULT getSoftwareFormat(out int samplerate, out SPEAKERMODE speakermode, out int numrawspeakers)
        {
            return FMOD5_System_GetSoftwareFormat(this.handle, out samplerate, out speakermode, out numrawspeakers);
        }
        public RESULT setDSPBufferSize(uint bufferlength, int numbuffers)
        {
            return FMOD5_System_SetDSPBufferSize(this.handle, bufferlength, numbuffers);
        }
        public RESULT getDSPBufferSize(out uint bufferlength, out int numbuffers)
        {
            return FMOD5_System_GetDSPBufferSize(this.handle, out bufferlength, out numbuffers);
        }
        public RESULT setFileSystem(FILE_OPEN_CALLBACK useropen, FILE_CLOSE_CALLBACK userclose, FILE_READ_CALLBACK userread, FILE_SEEK_CALLBACK userseek, FILE_ASYNCREAD_CALLBACK userasyncread, FILE_ASYNCCANCEL_CALLBACK userasynccancel, int blockalign)
        {
            return FMOD5_System_SetFileSystem(this.handle, useropen, userclose, userread, userseek, userasyncread, userasynccancel, blockalign);
        }
        public RESULT attachFileSystem(FILE_OPEN_CALLBACK useropen, FILE_CLOSE_CALLBACK userclose, FILE_READ_CALLBACK userread, FILE_SEEK_CALLBACK userseek)
        {
            return FMOD5_System_AttachFileSystem(this.handle, useropen, userclose, userread, userseek);
        }
        public RESULT setAdvancedSettings(ref ADVANCEDSETTINGS settings)
        {
            settings.cbSize = Marshal.SizeOf(settings);
            return FMOD5_System_SetAdvancedSettings(this.handle, ref settings);
        }
        public RESULT getAdvancedSettings(ref ADVANCEDSETTINGS settings)
        {
            settings.cbSize = Marshal.SizeOf(settings);
            return FMOD5_System_GetAdvancedSettings(this.handle, ref settings);
        }
        public RESULT setCallback(SYSTEM_CALLBACK callback, SYSTEM_CALLBACK_TYPE callbackmask = SYSTEM_CALLBACK_TYPE.ALL)
        {
            return FMOD5_System_SetCallback(this.handle, callback, callbackmask);
        }

        // Plug-in support.
        public RESULT setPluginPath(string path)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_SetPluginPath(this.handle, encoder.byteFromStringUTF8(path));
            }
        }
        public RESULT loadPlugin(string filename, out uint handle, uint priority = 0)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_LoadPlugin(this.handle, encoder.byteFromStringUTF8(filename), out handle, priority);
            }
        }
        public RESULT unloadPlugin(uint handle)
        {
            return FMOD5_System_UnloadPlugin(this.handle, handle);
        }
        public RESULT getNumNestedPlugins(uint handle, out int count)
        {
            return FMOD5_System_GetNumNestedPlugins(this.handle, handle, out count);
        }
        public RESULT getNestedPlugin(uint handle, int index, out uint nestedhandle)
        {
            return FMOD5_System_GetNestedPlugin(this.handle, handle, index, out nestedhandle);
        }
        public RESULT getNumPlugins(PLUGINTYPE plugintype, out int numplugins)
        {
            return FMOD5_System_GetNumPlugins(this.handle, plugintype, out numplugins);
        }
        public RESULT getPluginHandle(PLUGINTYPE plugintype, int index, out uint handle)
        {
            return FMOD5_System_GetPluginHandle(this.handle, plugintype, index, out handle);
        }
        public RESULT getPluginInfo(uint handle, out PLUGINTYPE plugintype, out string name, int namelen, out uint version)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_System_GetPluginInfo(this.handle, handle, out plugintype, stringMem, namelen, out version);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getPluginInfo(uint handle, out PLUGINTYPE plugintype, out uint version)
        {
            return FMOD5_System_GetPluginInfo(this.handle, handle, out plugintype, IntPtr.Zero, 0, out version);
        }
        public RESULT setOutputByPlugin(uint handle)
        {
            return FMOD5_System_SetOutputByPlugin(this.handle, handle);
        }
        public RESULT getOutputByPlugin(out uint handle)
        {
            return FMOD5_System_GetOutputByPlugin(this.handle, out handle);
        }
        public RESULT createDSPByPlugin(uint handle, out DSP dsp)
        {
            return FMOD5_System_CreateDSPByPlugin(this.handle, handle, out dsp.handle);
        }
        public RESULT getDSPInfoByPlugin(uint handle, out IntPtr description)
        {
            return FMOD5_System_GetDSPInfoByPlugin(this.handle, handle, out description);
        }
        /*
        public RESULT registerCodec          (ref CODEC_DESCRIPTION description, out uint handle, uint priority)
        public RESULT registerCodec          (ref CODEC_DESCRIPTION description, out uint handle, uint priority = 0)
        public RESULT registerCodec(ref CODEC_DESCRIPTION description, out uint handle, uint priority)
        {
            return FMOD5_System_RegisterCodec(this.handle, ref description, out handle, priority);
        }
        */
        public RESULT registerDSP(ref DSP_DESCRIPTION description, out uint handle)
        {
            return FMOD5_System_RegisterDSP(this.handle, ref description, out handle);
        }
        /*
        public RESULT registerOutput(ref OUTPUT_DESCRIPTION description, out uint handle)
        {
            return FMOD5_System_RegisterOutput(this.handle, ref description, out handle);
        }
        */

        // Init/Close.
        public RESULT init(int maxchannels, INITFLAGS flags, IntPtr extradriverdata)
        {
            return FMOD5_System_Init(this.handle, maxchannels, flags, extradriverdata);
        }
        public RESULT close()
        {
            return FMOD5_System_Close(this.handle);
        }

        // General post-init system functions.
        public RESULT update()
        {
            return FMOD5_System_Update(this.handle);
        }
        public RESULT setSpeakerPosition(SPEAKER speaker, float x, float y, bool active)
        {
            return FMOD5_System_SetSpeakerPosition(this.handle, speaker, x, y, active);
        }
        public RESULT getSpeakerPosition(SPEAKER speaker, out float x, out float y, out bool active)
        {
            return FMOD5_System_GetSpeakerPosition(this.handle, speaker, out x, out y, out active);
        }
        public RESULT setStreamBufferSize(uint filebuffersize, TIMEUNIT filebuffersizetype)
        {
            return FMOD5_System_SetStreamBufferSize(this.handle, filebuffersize, filebuffersizetype);
        }
        public RESULT getStreamBufferSize(out uint filebuffersize, out TIMEUNIT filebuffersizetype)
        {
            return FMOD5_System_GetStreamBufferSize(this.handle, out filebuffersize, out filebuffersizetype);
        }
        public RESULT set3DSettings(float dopplerscale, float distancefactor, float rolloffscale)
        {
            return FMOD5_System_Set3DSettings(this.handle, dopplerscale, distancefactor, rolloffscale);
        }
        public RESULT get3DSettings(out float dopplerscale, out float distancefactor, out float rolloffscale)
        {
            return FMOD5_System_Get3DSettings(this.handle, out dopplerscale, out distancefactor, out rolloffscale);
        }
        public RESULT set3DNumListeners(int numlisteners)
        {
            return FMOD5_System_Set3DNumListeners(this.handle, numlisteners);
        }
        public RESULT get3DNumListeners(out int numlisteners)
        {
            return FMOD5_System_Get3DNumListeners(this.handle, out numlisteners);
        }
        public RESULT set3DListenerAttributes(int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up)
        {
            return FMOD5_System_Set3DListenerAttributes(this.handle, listener, ref pos, ref vel, ref forward, ref up);
        }
        public RESULT get3DListenerAttributes(int listener, out VECTOR pos, out VECTOR vel, out VECTOR forward, out VECTOR up)
        {
            return FMOD5_System_Get3DListenerAttributes(this.handle, listener, out pos, out vel, out forward, out up);
        }
        public RESULT set3DRolloffCallback(CB_3D_ROLLOFF_CALLBACK callback)
        {
            return FMOD5_System_Set3DRolloffCallback(this.handle, callback);
        }
        public RESULT mixerSuspend()
        {
            return FMOD5_System_MixerSuspend(this.handle);
        }
        public RESULT mixerResume()
        {
            return FMOD5_System_MixerResume(this.handle);
        }
        public RESULT getDefaultMixMatrix(SPEAKERMODE sourcespeakermode, SPEAKERMODE targetspeakermode, float[] matrix, int matrixhop)
        {
            return FMOD5_System_GetDefaultMixMatrix(this.handle, sourcespeakermode, targetspeakermode, matrix, matrixhop);
        }
        public RESULT getSpeakerModeChannels(SPEAKERMODE mode, out int channels)
        {
            return FMOD5_System_GetSpeakerModeChannels(this.handle, mode, out channels);
        }

        // System information functions.
        public RESULT getVersion(out uint version)
        {
            return FMOD5_System_GetVersion(this.handle, out version);
        }
        public RESULT getOutputHandle(out IntPtr handle)
        {
            return FMOD5_System_GetOutputHandle(this.handle, out handle);
        }
        public RESULT getChannelsPlaying(out int channels)
        {
            return FMOD5_System_GetChannelsPlaying(this.handle, out channels, IntPtr.Zero);
        }
        public RESULT getChannelsPlaying(out int channels, out int realchannels)
        {
            return FMOD5_System_GetChannelsPlaying(this.handle, out channels, out realchannels);
        }
        public RESULT getCPUUsage(out float dsp, out float stream, out float geometry, out float update, out float total)
        {
            return FMOD5_System_GetCPUUsage(this.handle, out dsp, out stream, out geometry, out update, out total);
        }
        public RESULT getFileUsage(out Int64 sampleBytesRead, out Int64 streamBytesRead, out Int64 otherBytesRead)
        {
            return FMOD5_System_GetFileUsage(this.handle, out sampleBytesRead, out streamBytesRead, out otherBytesRead);
        }

        // Sound/DSP/Channel/FX creation and retrieval.
        public RESULT createSound(string name, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                 return FMOD5_System_CreateSound(this.handle, encoder.byteFromStringUTF8(name), mode, ref exinfo, out sound.handle);
            }
        }
        public RESULT createSound(byte[] data, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            return FMOD5_System_CreateSound(this.handle, data, mode, ref exinfo, out sound.handle);
        }
        public RESULT createSound(IntPtr name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            return FMOD5_System_CreateSound(this.handle, name_or_data, mode, ref exinfo, out sound.handle);
        }
        public RESULT createSound(string name, MODE mode, out Sound sound)
        {
            CREATESOUNDEXINFO exinfo = new CREATESOUNDEXINFO();
            exinfo.cbsize = Marshal.SizeOf(exinfo);

            return createSound(name, mode, ref exinfo, out sound);
        }
        public RESULT createStream(string name, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_CreateStream(this.handle, encoder.byteFromStringUTF8(name), mode, ref exinfo, out sound.handle);
            }
        }
        public RESULT createStream(byte[] data, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            return FMOD5_System_CreateStream(this.handle, data, mode, ref exinfo, out sound.handle);
        }
        public RESULT createStream(IntPtr name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out Sound sound)
        {
            return FMOD5_System_CreateStream(this.handle, name_or_data, mode, ref exinfo, out sound.handle);
        }
        public RESULT createStream(string name, MODE mode, out Sound sound)
        {
            CREATESOUNDEXINFO exinfo = new CREATESOUNDEXINFO();
            exinfo.cbsize = Marshal.SizeOf(exinfo);

            return createStream(name, mode, ref exinfo, out sound);
        }
        public RESULT createDSP(ref DSP_DESCRIPTION description, out DSP dsp)
        {
            return FMOD5_System_CreateDSP(this.handle, ref description, out dsp.handle);
        }
        public RESULT createDSPByType(DSP_TYPE type, out DSP dsp)
        {
            return FMOD5_System_CreateDSPByType(this.handle, type, out dsp.handle);
        }
        public RESULT createChannelGroup(string name, out ChannelGroup channelgroup)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_CreateChannelGroup(this.handle, encoder.byteFromStringUTF8(name), out channelgroup.handle);
            }
        }
        public RESULT createSoundGroup(string name, out SoundGroup soundgroup)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_CreateSoundGroup(this.handle, encoder.byteFromStringUTF8(name), out soundgroup.handle);
            }
        }
        public RESULT createReverb3D(out Reverb3D reverb)
        {
            return FMOD5_System_CreateReverb3D(this.handle, out reverb.handle);
        }
        public RESULT playSound(Sound sound, ChannelGroup channelgroup, bool paused, out Channel channel)
        {
            return FMOD5_System_PlaySound(this.handle, sound.handle, channelgroup.handle, paused, out channel.handle);
        }
        public RESULT playDSP(DSP dsp, ChannelGroup channelgroup, bool paused, out Channel channel)
        {
            return FMOD5_System_PlayDSP(this.handle, dsp.handle, channelgroup.handle, paused, out channel.handle);
        }
        public RESULT getChannel(int channelid, out Channel channel)
        {
            return FMOD5_System_GetChannel(this.handle, channelid, out channel.handle);
        }
        public RESULT getMasterChannelGroup(out ChannelGroup channelgroup)
        {
            return FMOD5_System_GetMasterChannelGroup(this.handle, out channelgroup.handle);
        }
        public RESULT getMasterSoundGroup(out SoundGroup soundgroup)
        {
            return FMOD5_System_GetMasterSoundGroup(this.handle, out soundgroup.handle);
        }

        // Routing to ports.
        public RESULT attachChannelGroupToPort(uint portType, ulong portIndex, ChannelGroup channelgroup, bool passThru = false)
        {
            return FMOD5_System_AttachChannelGroupToPort(this.handle, portType, portIndex, channelgroup.handle, passThru);
        }
        public RESULT detachChannelGroupFromPort(ChannelGroup channelgroup)
        {
            return FMOD5_System_DetachChannelGroupFromPort(this.handle, channelgroup.handle);
        }

        // Reverb api.
        public RESULT setReverbProperties(int instance, ref REVERB_PROPERTIES prop)
        {
            return FMOD5_System_SetReverbProperties(this.handle, instance, ref prop);
        }
        public RESULT getReverbProperties(int instance, out REVERB_PROPERTIES prop)
        {
            return FMOD5_System_GetReverbProperties(this.handle, instance, out prop);
        }

        // System level DSP functionality.
        public RESULT lockDSP()
        {
            return FMOD5_System_LockDSP(this.handle);
        }
        public RESULT unlockDSP()
        {
            return FMOD5_System_UnlockDSP(this.handle);
        }

        // Recording api
        public RESULT getRecordNumDrivers(out int numdrivers, out int numconnected)
        {
            return FMOD5_System_GetRecordNumDrivers(this.handle, out numdrivers, out numconnected);
        }
        public RESULT getRecordDriverInfo(int id, out string name, int namelen, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels, out DRIVER_STATE state)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_System_GetRecordDriverInfo(this.handle, id, stringMem, namelen, out guid, out systemrate, out speakermode, out speakermodechannels, out state);

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getRecordDriverInfo(int id, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels, out DRIVER_STATE state)
        {
            return FMOD5_System_GetRecordDriverInfo(this.handle, id, IntPtr.Zero, 0, out guid, out systemrate, out speakermode, out speakermodechannels, out state);
        }
        public RESULT getRecordPosition(int id, out uint position)
        {
            return FMOD5_System_GetRecordPosition(this.handle, id, out position);
        }
        public RESULT recordStart(int id, Sound sound, bool loop)
        {
            return FMOD5_System_RecordStart(this.handle, id, sound.handle, loop);
        }
        public RESULT recordStop(int id)
        {
            return FMOD5_System_RecordStop(this.handle, id);
        }
        public RESULT isRecording(int id, out bool recording)
        {
            return FMOD5_System_IsRecording(this.handle, id, out recording);
        }

        // Geometry api
        public RESULT createGeometry(int maxpolygons, int maxvertices, out Geometry geometry)
        {
            return FMOD5_System_CreateGeometry(this.handle, maxpolygons, maxvertices, out geometry.handle);
        }
        public RESULT setGeometrySettings(float maxworldsize)
        {
            return FMOD5_System_SetGeometrySettings(this.handle, maxworldsize);
        }
        public RESULT getGeometrySettings(out float maxworldsize)
        {
            return FMOD5_System_GetGeometrySettings(this.handle, out maxworldsize);
        }
        public RESULT loadGeometry(IntPtr data, int datasize, out Geometry geometry)
        {
            return FMOD5_System_LoadGeometry(this.handle, data, datasize, out geometry.handle);
        }
        public RESULT getGeometryOcclusion(ref VECTOR listener, ref VECTOR source, out float direct, out float reverb)
        {
            return FMOD5_System_GetGeometryOcclusion(this.handle, ref listener, ref source, out direct, out reverb);
        }

        // Network functions
        public RESULT setNetworkProxy(string proxy)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_System_SetNetworkProxy(this.handle, encoder.byteFromStringUTF8(proxy));
            }
        }
        public RESULT getNetworkProxy(out string proxy, int proxylen)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(proxylen);

            RESULT result = FMOD5_System_GetNetworkProxy(this.handle, stringMem, proxylen);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                proxy = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT setNetworkTimeout(int timeout)
        {
            return FMOD5_System_SetNetworkTimeout(this.handle, timeout);
        }
        public RESULT getNetworkTimeout(out int timeout)
        {
            return FMOD5_System_GetNetworkTimeout(this.handle, out timeout);
        }

        // Userdata set/get
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_System_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_System_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Release                   (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetOutput                 (IntPtr system, OUTPUTTYPE output);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetOutput                 (IntPtr system, out OUTPUTTYPE output);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNumDrivers             (IntPtr system, out int numdrivers);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetDriverInfo             (IntPtr system, int id, IntPtr name, int namelen, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetDriver                 (IntPtr system, int driver);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetDriver                 (IntPtr system, out int driver);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetSoftwareChannels       (IntPtr system, int numsoftwarechannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetSoftwareChannels       (IntPtr system, out int numsoftwarechannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetSoftwareFormat         (IntPtr system, int samplerate, SPEAKERMODE speakermode, int numrawspeakers);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetSoftwareFormat         (IntPtr system, out int samplerate, out SPEAKERMODE speakermode, out int numrawspeakers);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetDSPBufferSize          (IntPtr system, uint bufferlength, int numbuffers);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetDSPBufferSize          (IntPtr system, out uint bufferlength, out int numbuffers);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetFileSystem             (IntPtr system, FILE_OPEN_CALLBACK useropen, FILE_CLOSE_CALLBACK userclose, FILE_READ_CALLBACK userread, FILE_SEEK_CALLBACK userseek, FILE_ASYNCREAD_CALLBACK userasyncread, FILE_ASYNCCANCEL_CALLBACK userasynccancel, int blockalign);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_AttachFileSystem          (IntPtr system, FILE_OPEN_CALLBACK useropen, FILE_CLOSE_CALLBACK userclose, FILE_READ_CALLBACK userread, FILE_SEEK_CALLBACK userseek);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetAdvancedSettings       (IntPtr system, ref ADVANCEDSETTINGS settings);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetAdvancedSettings       (IntPtr system, ref ADVANCEDSETTINGS settings);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetCallback               (IntPtr system, SYSTEM_CALLBACK callback, SYSTEM_CALLBACK_TYPE callbackmask);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetPluginPath             (IntPtr system, byte[] path);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_LoadPlugin                (IntPtr system, byte[] filename, out uint handle, uint priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_UnloadPlugin              (IntPtr system, uint handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNumNestedPlugins       (IntPtr system, uint handle, out int count);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNestedPlugin           (IntPtr system, uint handle, int index, out uint nestedhandle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNumPlugins             (IntPtr system, PLUGINTYPE plugintype, out int numplugins);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetPluginHandle           (IntPtr system, PLUGINTYPE plugintype, int index, out uint handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetPluginInfo             (IntPtr system, uint handle, out PLUGINTYPE plugintype, IntPtr name, int namelen, out uint version);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetOutputByPlugin         (IntPtr system, uint handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetOutputByPlugin         (IntPtr system, out uint handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateDSPByPlugin         (IntPtr system, uint handle, out IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetDSPInfoByPlugin        (IntPtr system, uint handle, out IntPtr description);
        //[DllImport(VERSION.dll)]
        //private static extern RESULT FMOD5_System_RegisterCodec           (IntPtr system, out CODEC_DESCRIPTION description, out uint handle, uint priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_RegisterDSP               (IntPtr system, ref DSP_DESCRIPTION description, out uint handle);
        //[DllImport(VERSION.dll)]
        //private static extern RESULT FMOD5_System_RegisterOutput          (IntPtr system, ref OUTPUT_DESCRIPTION description, out uint handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Init                      (IntPtr system, int maxchannels, INITFLAGS flags, IntPtr extradriverdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Close                     (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Update                    (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetSpeakerPosition        (IntPtr system, SPEAKER speaker, float x, float y, bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetSpeakerPosition        (IntPtr system, SPEAKER speaker, out float x, out float y, out bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetStreamBufferSize       (IntPtr system, uint filebuffersize, TIMEUNIT filebuffersizetype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetStreamBufferSize       (IntPtr system, out uint filebuffersize, out TIMEUNIT filebuffersizetype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Set3DSettings             (IntPtr system, float dopplerscale, float distancefactor, float rolloffscale);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Get3DSettings             (IntPtr system, out float dopplerscale, out float distancefactor, out float rolloffscale);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Set3DNumListeners         (IntPtr system, int numlisteners);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Get3DNumListeners         (IntPtr system, out int numlisteners);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Set3DListenerAttributes   (IntPtr system, int listener, ref VECTOR pos, ref VECTOR vel, ref VECTOR forward, ref VECTOR up);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Get3DListenerAttributes   (IntPtr system, int listener, out VECTOR pos, out VECTOR vel, out VECTOR forward, out VECTOR up);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_Set3DRolloffCallback      (IntPtr system, CB_3D_ROLLOFF_CALLBACK callback);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_MixerSuspend              (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_MixerResume               (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetDefaultMixMatrix       (IntPtr system, SPEAKERMODE sourcespeakermode, SPEAKERMODE targetspeakermode, float[] matrix, int matrixhop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetSpeakerModeChannels    (IntPtr system, SPEAKERMODE mode, out int channels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetVersion                (IntPtr system, out uint version);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetOutputHandle           (IntPtr system, out IntPtr handle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetChannelsPlaying        (IntPtr system, out int channels, IntPtr zero);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetChannelsPlaying        (IntPtr system, out int channels, out int realchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetCPUUsage               (IntPtr system, out float dsp, out float stream, out float geometry, out float update, out float total);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetFileUsage              (IntPtr system, out Int64 sampleBytesRead, out Int64 streamBytesRead, out Int64 otherBytesRead);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateSound               (IntPtr system, byte[] name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateSound               (IntPtr system, IntPtr name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateStream              (IntPtr system, byte[] name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateStream              (IntPtr system, IntPtr name_or_data, MODE mode, ref CREATESOUNDEXINFO exinfo, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateDSP                 (IntPtr system, ref DSP_DESCRIPTION description, out IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateDSPByType           (IntPtr system, DSP_TYPE type, out IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateChannelGroup        (IntPtr system, byte[] name, out IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateSoundGroup          (IntPtr system, byte[] name, out IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateReverb3D            (IntPtr system, out IntPtr reverb);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_PlaySound                 (IntPtr system, IntPtr sound, IntPtr channelgroup, bool paused, out IntPtr channel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_PlayDSP                   (IntPtr system, IntPtr dsp, IntPtr channelgroup, bool paused, out IntPtr channel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetChannel                (IntPtr system, int channelid, out IntPtr channel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetMasterChannelGroup     (IntPtr system, out IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetMasterSoundGroup       (IntPtr system, out IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_AttachChannelGroupToPort  (IntPtr system, uint portType, ulong portIndex, IntPtr channelgroup, bool passThru);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_DetachChannelGroupFromPort(IntPtr system, IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetReverbProperties       (IntPtr system, int instance, ref REVERB_PROPERTIES prop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetReverbProperties       (IntPtr system, int instance, out REVERB_PROPERTIES prop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_LockDSP                   (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_UnlockDSP                 (IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetRecordNumDrivers       (IntPtr system, out int numdrivers, out int numconnected);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetRecordDriverInfo       (IntPtr system, int id, IntPtr name, int namelen, out Guid guid, out int systemrate, out SPEAKERMODE speakermode, out int speakermodechannels, out DRIVER_STATE state);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetRecordPosition         (IntPtr system, int id, out uint position);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_RecordStart               (IntPtr system, int id, IntPtr sound, bool loop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_RecordStop                (IntPtr system, int id);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_IsRecording               (IntPtr system, int id, out bool recording);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_CreateGeometry            (IntPtr system, int maxpolygons, int maxvertices, out IntPtr geometry);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetGeometrySettings       (IntPtr system, float maxworldsize);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetGeometrySettings       (IntPtr system, out float maxworldsize);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_LoadGeometry              (IntPtr system, IntPtr data, int datasize, out IntPtr geometry);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetGeometryOcclusion      (IntPtr system, ref VECTOR listener, ref VECTOR source, out float direct, out float reverb);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetNetworkProxy           (IntPtr system, byte[] proxy);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNetworkProxy           (IntPtr system, IntPtr proxy, int proxylen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetNetworkTimeout         (IntPtr system, int timeout);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetNetworkTimeout         (IntPtr system, out int timeout);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_SetUserData               (IntPtr system, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_System_GetUserData               (IntPtr system, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }


    /*
        'Sound' API.
    */
    public struct Sound
    {
        public RESULT release()
        {
            return FMOD5_Sound_Release(this.handle);
        }
        public RESULT getSystemObject(out System system)
        {
            return FMOD5_Sound_GetSystemObject(this.handle, out system.handle);
        }

        // Standard sound manipulation functions.
        public RESULT @lock(uint offset, uint length, out IntPtr ptr1, out IntPtr ptr2, out uint len1, out uint len2)
        {
            return FMOD5_Sound_Lock(this.handle, offset, length, out ptr1, out ptr2, out len1, out len2);
        }
        public RESULT unlock(IntPtr ptr1, IntPtr ptr2, uint len1, uint len2)
        {
            return FMOD5_Sound_Unlock(this.handle, ptr1, ptr2, len1, len2);
        }
        public RESULT setDefaults(float frequency, int priority)
        {
            return FMOD5_Sound_SetDefaults(this.handle, frequency, priority);
        }
        public RESULT getDefaults(out float frequency, out int priority)
        {
            return FMOD5_Sound_GetDefaults(this.handle, out frequency, out priority);
        }
        public RESULT set3DMinMaxDistance(float min, float max)
        {
            return FMOD5_Sound_Set3DMinMaxDistance(this.handle, min, max);
        }
        public RESULT get3DMinMaxDistance(out float min, out float max)
        {
            return FMOD5_Sound_Get3DMinMaxDistance(this.handle, out min, out max);
        }
        public RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
        {
            return FMOD5_Sound_Set3DConeSettings(this.handle, insideconeangle, outsideconeangle, outsidevolume);
        }
        public RESULT get3DConeSettings(out float insideconeangle, out float outsideconeangle, out float outsidevolume)
        {
            return FMOD5_Sound_Get3DConeSettings(this.handle, out insideconeangle, out outsideconeangle, out outsidevolume);
        }
        public RESULT set3DCustomRolloff(ref VECTOR points, int numpoints)
        {
            return FMOD5_Sound_Set3DCustomRolloff(this.handle, ref points, numpoints);
        }
        public RESULT get3DCustomRolloff(out IntPtr points, out int numpoints)
        {
            return FMOD5_Sound_Get3DCustomRolloff(this.handle, out points, out numpoints);
        }

        public RESULT getSubSound(int index, out Sound subsound)
        {
            return FMOD5_Sound_GetSubSound(this.handle, index, out subsound.handle);
        }
        public RESULT getSubSoundParent(out Sound parentsound)
        {
            return FMOD5_Sound_GetSubSoundParent(this.handle, out parentsound.handle);
        }
        public RESULT getName(out string name, int namelen)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_Sound_GetName(this.handle, stringMem, namelen);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getLength(out uint length, TIMEUNIT lengthtype)
        {
            return FMOD5_Sound_GetLength(this.handle, out length, lengthtype);
        }
        public RESULT getFormat(out SOUND_TYPE type, out SOUND_FORMAT format, out int channels, out int bits)
        {
            return FMOD5_Sound_GetFormat(this.handle, out type, out format, out channels, out bits);
        }
        public RESULT getNumSubSounds(out int numsubsounds)
        {
            return FMOD5_Sound_GetNumSubSounds(this.handle, out numsubsounds);
        }
        public RESULT getNumTags(out int numtags, out int numtagsupdated)
        {
            return FMOD5_Sound_GetNumTags(this.handle, out numtags, out numtagsupdated);
        }
        public RESULT getTag(string name, int index, out TAG tag)
        {
             using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_Sound_GetTag(this.handle, encoder.byteFromStringUTF8(name), index, out tag);
            }
        }
        public RESULT getOpenState(out OPENSTATE openstate, out uint percentbuffered, out bool starving, out bool diskbusy)
        {
            return FMOD5_Sound_GetOpenState(this.handle, out openstate, out percentbuffered, out starving, out diskbusy);
        }
        public RESULT readData(IntPtr buffer, uint length, out uint read)
        {
            return FMOD5_Sound_ReadData(this.handle, buffer, length, out read);
        }
        public RESULT seekData(uint pcm)
        {
            return FMOD5_Sound_SeekData(this.handle, pcm);
        }
        public RESULT setSoundGroup(SoundGroup soundgroup)
        {
            return FMOD5_Sound_SetSoundGroup(this.handle, soundgroup.handle);
        }
        public RESULT getSoundGroup(out SoundGroup soundgroup)
        {
            return FMOD5_Sound_GetSoundGroup(this.handle, out soundgroup.handle);
        }

        // Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.
        public RESULT getNumSyncPoints(out int numsyncpoints)
        {
            return FMOD5_Sound_GetNumSyncPoints(this.handle, out numsyncpoints);
        }
        public RESULT getSyncPoint(int index, out IntPtr point)
        {
            return FMOD5_Sound_GetSyncPoint(this.handle, index, out point);
        }
        public RESULT getSyncPointInfo(IntPtr point, out string name, int namelen, out uint offset, TIMEUNIT offsettype)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_Sound_GetSyncPointInfo(this.handle, point, stringMem, namelen, out offset, offsettype);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getSyncPointInfo(IntPtr point, out uint offset, TIMEUNIT offsettype)
        {
            return FMOD5_Sound_GetSyncPointInfo(this.handle, point, IntPtr.Zero, 0, out offset, offsettype);
        }
        public RESULT addSyncPoint(uint offset, TIMEUNIT offsettype, string name, out IntPtr point)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD5_Sound_AddSyncPoint(this.handle, offset, offsettype, encoder.byteFromStringUTF8(name), out point);
            }
        }
        public RESULT deleteSyncPoint(IntPtr point)
        {
            return FMOD5_Sound_DeleteSyncPoint(this.handle, point);
        }

        // Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
        public RESULT setMode(MODE mode)
        {
            return FMOD5_Sound_SetMode(this.handle, mode);
        }
        public RESULT getMode(out MODE mode)
        {
            return FMOD5_Sound_GetMode(this.handle, out mode);
        }
        public RESULT setLoopCount(int loopcount)
        {
            return FMOD5_Sound_SetLoopCount(this.handle, loopcount);
        }
        public RESULT getLoopCount(out int loopcount)
        {
            return FMOD5_Sound_GetLoopCount(this.handle, out loopcount);
        }
        public RESULT setLoopPoints(uint loopstart, TIMEUNIT loopstarttype, uint loopend, TIMEUNIT loopendtype)
        {
            return FMOD5_Sound_SetLoopPoints(this.handle, loopstart, loopstarttype, loopend, loopendtype);
        }
        public RESULT getLoopPoints(out uint loopstart, TIMEUNIT loopstarttype, out uint loopend, TIMEUNIT loopendtype)
        {
            return FMOD5_Sound_GetLoopPoints(this.handle, out loopstart, loopstarttype, out loopend, loopendtype);
        }

        // For MOD/S3M/XM/IT/MID sequenced formats only.
        public RESULT getMusicNumChannels(out int numchannels)
        {
            return FMOD5_Sound_GetMusicNumChannels(this.handle, out numchannels);
        }
        public RESULT setMusicChannelVolume(int channel, float volume)
        {
            return FMOD5_Sound_SetMusicChannelVolume(this.handle, channel, volume);
        }
        public RESULT getMusicChannelVolume(int channel, out float volume)
        {
            return FMOD5_Sound_GetMusicChannelVolume(this.handle, channel, out volume);
        }
        public RESULT setMusicSpeed(float speed)
        {
            return FMOD5_Sound_SetMusicSpeed(this.handle, speed);
        }
        public RESULT getMusicSpeed(out float speed)
        {
            return FMOD5_Sound_GetMusicSpeed(this.handle, out speed);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_Sound_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_Sound_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Release                 (IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSystemObject         (IntPtr sound, out IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Lock                    (IntPtr sound, uint offset, uint length, out IntPtr ptr1, out IntPtr ptr2, out uint len1, out uint len2);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Unlock                  (IntPtr sound, IntPtr ptr1,  IntPtr ptr2, uint len1, uint len2);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetDefaults             (IntPtr sound, float frequency, int priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetDefaults             (IntPtr sound, out float frequency, out int priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Set3DMinMaxDistance     (IntPtr sound, float min, float max);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Get3DMinMaxDistance     (IntPtr sound, out float min, out float max);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Set3DConeSettings       (IntPtr sound, float insideconeangle, float outsideconeangle, float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Get3DConeSettings       (IntPtr sound, out float insideconeangle, out float outsideconeangle, out float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Set3DCustomRolloff      (IntPtr sound, ref VECTOR points, int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_Get3DCustomRolloff      (IntPtr sound, out IntPtr points, out int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSubSound             (IntPtr sound, int index, out IntPtr subsound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSubSoundParent       (IntPtr sound, out IntPtr parentsound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetName                 (IntPtr sound, IntPtr name, int namelen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetLength               (IntPtr sound, out uint length, TIMEUNIT lengthtype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetFormat               (IntPtr sound, out SOUND_TYPE type, out SOUND_FORMAT format, out int channels, out int bits);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetNumSubSounds         (IntPtr sound, out int numsubsounds);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetNumTags              (IntPtr sound, out int numtags, out int numtagsupdated);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetTag                  (IntPtr sound, byte[] name, int index, out TAG tag);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetOpenState            (IntPtr sound, out OPENSTATE openstate, out uint percentbuffered, out bool starving, out bool diskbusy);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_ReadData                (IntPtr sound, IntPtr buffer, uint length, out uint read);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SeekData                (IntPtr sound, uint pcm);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetSoundGroup           (IntPtr sound, IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSoundGroup           (IntPtr sound, out IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetNumSyncPoints        (IntPtr sound, out int numsyncpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSyncPoint            (IntPtr sound, int index, out IntPtr point);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetSyncPointInfo        (IntPtr sound, IntPtr point, IntPtr name, int namelen, out uint offset, TIMEUNIT offsettype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_AddSyncPoint            (IntPtr sound, uint offset, TIMEUNIT offsettype, byte[] name, out IntPtr point);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_DeleteSyncPoint         (IntPtr sound, IntPtr point);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetMode                 (IntPtr sound, MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetMode                 (IntPtr sound, out MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetLoopCount            (IntPtr sound, int loopcount);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetLoopCount            (IntPtr sound, out int loopcount);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetLoopPoints           (IntPtr sound, uint loopstart, TIMEUNIT loopstarttype, uint loopend, TIMEUNIT loopendtype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetLoopPoints           (IntPtr sound, out uint loopstart, TIMEUNIT loopstarttype, out uint loopend, TIMEUNIT loopendtype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetMusicNumChannels     (IntPtr sound, out int numchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetMusicChannelVolume   (IntPtr sound, int channel, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetMusicChannelVolume   (IntPtr sound, int channel, out float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetMusicSpeed           (IntPtr sound, float speed);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetMusicSpeed           (IntPtr sound, out float speed);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_SetUserData             (IntPtr sound, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Sound_GetUserData             (IntPtr sound, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'ChannelControl' API
    */
    interface IChannelControl
    {
        RESULT getSystemObject              (out System system);

        // General control functionality for Channels and ChannelGroups.
        RESULT stop                         ();
        RESULT setPaused                    (bool paused);
        RESULT getPaused                    (out bool paused);
        RESULT setVolume                    (float volume);
        RESULT getVolume                    (out float volume);
        RESULT setVolumeRamp                (bool ramp);
        RESULT getVolumeRamp                (out bool ramp);
        RESULT getAudibility                (out float audibility);
        RESULT setPitch                     (float pitch);
        RESULT getPitch                     (out float pitch);
        RESULT setMute                      (bool mute);
        RESULT getMute                      (out bool mute);
        RESULT setReverbProperties          (int instance, float wet);
        RESULT getReverbProperties          (int instance, out float wet);
        RESULT setLowPassGain               (float gain);
        RESULT getLowPassGain               (out float gain);
        RESULT setMode                      (MODE mode);
        RESULT getMode                      (out MODE mode);
        RESULT setCallback                  (CHANNELCONTROL_CALLBACK callback);
        RESULT isPlaying                    (out bool isplaying);

        // Panning and level adjustment.
        RESULT setPan                       (float pan);
        RESULT setMixLevelsOutput           (float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
        RESULT setMixLevelsInput            (float[] levels, int numlevels);
        RESULT setMixMatrix                 (float[] matrix, int outchannels, int inchannels, int inchannel_hop);
        RESULT getMixMatrix                 (float[] matrix, out int outchannels, out int inchannels, int inchannel_hop);

        // Clock based functionality.
        RESULT getDSPClock                  (out ulong dspclock, out ulong parentclock);
        RESULT setDelay                     (ulong dspclock_start, ulong dspclock_end, bool stopchannels);
        RESULT getDelay                     (out ulong dspclock_start, out ulong dspclock_end);
        RESULT getDelay                     (out ulong dspclock_start, out ulong dspclock_end, out bool stopchannels);
        RESULT addFadePoint                 (ulong dspclock, float volume);
        RESULT setFadePointRamp             (ulong dspclock, float volume);
        RESULT removeFadePoints             (ulong dspclock_start, ulong dspclock_end);
        RESULT getFadePoints                (ref uint numpoints, ulong[] point_dspclock, float[] point_volume);

        // DSP effects.
        RESULT getDSP                       (int index, out DSP dsp);
        RESULT addDSP                       (int index, DSP dsp);
        RESULT removeDSP                    (DSP dsp);
        RESULT getNumDSPs                   (out int numdsps);
        RESULT setDSPIndex                  (DSP dsp, int index);
        RESULT getDSPIndex                  (DSP dsp, out int index);

        // 3D functionality.
        RESULT set3DAttributes              (ref VECTOR pos, ref VECTOR vel);
        RESULT get3DAttributes              (out VECTOR pos, out VECTOR vel);
        RESULT set3DMinMaxDistance          (float mindistance, float maxdistance);
        RESULT get3DMinMaxDistance          (out float mindistance, out float maxdistance);
        RESULT set3DConeSettings            (float insideconeangle, float outsideconeangle, float outsidevolume);
        RESULT get3DConeSettings            (out float insideconeangle, out float outsideconeangle, out float outsidevolume);
        RESULT set3DConeOrientation         (ref VECTOR orientation);
        RESULT get3DConeOrientation         (out VECTOR orientation);
        RESULT set3DCustomRolloff           (ref VECTOR points, int numpoints);
        RESULT get3DCustomRolloff           (out IntPtr points, out int numpoints);
        RESULT set3DOcclusion               (float directocclusion, float reverbocclusion);
        RESULT get3DOcclusion               (out float directocclusion, out float reverbocclusion);
        RESULT set3DSpread                  (float angle);
        RESULT get3DSpread                  (out float angle);
        RESULT set3DLevel                   (float level);
        RESULT get3DLevel                   (out float level);
        RESULT set3DDopplerLevel            (float level);
        RESULT get3DDopplerLevel            (out float level);
        RESULT set3DDistanceFilter          (bool custom, float customLevel, float centerFreq);
        RESULT get3DDistanceFilter          (out bool custom, out float customLevel, out float centerFreq);

        // Userdata set/get.
        RESULT setUserData                  (IntPtr userdata);
        RESULT getUserData                  (out IntPtr userdata);
    }

    /*
        'Channel' API
    */
    public struct Channel : IChannelControl
    {
        // Channel specific control functionality.
        public RESULT setFrequency(float frequency)
        {
            return FMOD5_Channel_SetFrequency(this.handle, frequency);
        }
        public RESULT getFrequency(out float frequency)
        {
            return FMOD5_Channel_GetFrequency(this.handle, out frequency);
        }
        public RESULT setPriority(int priority)
        {
            return FMOD5_Channel_SetPriority(this.handle, priority);
        }
        public RESULT getPriority(out int priority)
        {
            return FMOD5_Channel_GetPriority(this.handle, out priority);
        }
        public RESULT setPosition(uint position, TIMEUNIT postype)
        {
            return FMOD5_Channel_SetPosition(this.handle, position, postype);
        }
        public RESULT getPosition(out uint position, TIMEUNIT postype)
        {
            return FMOD5_Channel_GetPosition(this.handle, out position, postype);
        }
        public RESULT setChannelGroup(ChannelGroup channelgroup)
        {
            return FMOD5_Channel_SetChannelGroup(this.handle, channelgroup.handle);
        }
        public RESULT getChannelGroup(out ChannelGroup channelgroup)
        {
            return FMOD5_Channel_GetChannelGroup(this.handle, out channelgroup.handle);
        }
        public RESULT setLoopCount(int loopcount)
        {
            return FMOD5_Channel_SetLoopCount(this.handle, loopcount);
        }
        public RESULT getLoopCount(out int loopcount)
        {
            return FMOD5_Channel_GetLoopCount(this.handle, out loopcount);
        }
        public RESULT setLoopPoints(uint loopstart, TIMEUNIT loopstarttype, uint loopend, TIMEUNIT loopendtype)
        {
            return FMOD5_Channel_SetLoopPoints(this.handle, loopstart, loopstarttype, loopend, loopendtype);
        }
        public RESULT getLoopPoints(out uint loopstart, TIMEUNIT loopstarttype, out uint loopend, TIMEUNIT loopendtype)
        {
            return FMOD5_Channel_GetLoopPoints(this.handle, out loopstart, loopstarttype, out loopend, loopendtype);
        }

        // Information only functions.
        public RESULT isVirtual(out bool isvirtual)
        {
            return FMOD5_Channel_IsVirtual(this.handle, out isvirtual);
        }
        public RESULT getCurrentSound(out Sound sound)
        {
            return FMOD5_Channel_GetCurrentSound(this.handle, out sound.handle);
        }
        public RESULT getIndex(out int index)
        {
            return FMOD5_Channel_GetIndex(this.handle, out index);
        }
        public RESULT getSystemObject(out System system)
        {
            return FMOD5_Channel_GetSystemObject(this.handle, out system.handle);
        }

        // General control functionality for Channels and ChannelGroups.
        public RESULT stop()
        {
            return FMOD5_Channel_Stop(this.handle);
        }
        public RESULT setPaused(bool paused)
        {
            return FMOD5_Channel_SetPaused(this.handle, paused);
        }
        public RESULT getPaused(out bool paused)
        {
            return FMOD5_Channel_GetPaused(this.handle, out paused);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD5_Channel_SetVolume(this.handle, volume);
        }
        public RESULT getVolume(out float volume)
        {
            return FMOD5_Channel_GetVolume(this.handle, out volume);
        }
        public RESULT setVolumeRamp(bool ramp)
        {
            return FMOD5_Channel_SetVolumeRamp(this.handle, ramp);
        }
        public RESULT getVolumeRamp(out bool ramp)
        {
            return FMOD5_Channel_GetVolumeRamp(this.handle, out ramp);
        }
        public RESULT getAudibility(out float audibility)
        {
            return FMOD5_Channel_GetAudibility(this.handle, out audibility);
        }
        public RESULT setPitch(float pitch)
        {
            return FMOD5_Channel_SetPitch(this.handle, pitch);
        }
        public RESULT getPitch(out float pitch)
        {
            return FMOD5_Channel_GetPitch(this.handle, out pitch);
        }
        public RESULT setMute(bool mute)
        {
            return FMOD5_Channel_SetMute(this.handle, mute);
        }
        public RESULT getMute(out bool mute)
        {
            return FMOD5_Channel_GetMute(this.handle, out mute);
        }
        public RESULT setReverbProperties(int instance, float wet)
        {
            return FMOD5_Channel_SetReverbProperties(this.handle, instance, wet);
        }
        public RESULT getReverbProperties(int instance, out float wet)
        {
            return FMOD5_Channel_GetReverbProperties(this.handle, instance, out wet);
        }
        public RESULT setLowPassGain(float gain)
        {
            return FMOD5_Channel_SetLowPassGain(this.handle, gain);
        }
        public RESULT getLowPassGain(out float gain)
        {
            return FMOD5_Channel_GetLowPassGain(this.handle, out gain);
        }
        public RESULT setMode(MODE mode)
        {
            return FMOD5_Channel_SetMode(this.handle, mode);
        }
        public RESULT getMode(out MODE mode)
        {
            return FMOD5_Channel_GetMode(this.handle, out mode);
        }
        public RESULT setCallback(CHANNELCONTROL_CALLBACK callback)
        {
            return FMOD5_Channel_SetCallback(this.handle, callback);
        }
        public RESULT isPlaying(out bool isplaying)
        {
            return FMOD5_Channel_IsPlaying(this.handle, out isplaying);
        }

        // Panning and level adjustment.
        public RESULT setPan(float pan)
        {
            return FMOD5_Channel_SetPan(this.handle, pan);
        }
        public RESULT setMixLevelsOutput(float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright)
        {
            return FMOD5_Channel_SetMixLevelsOutput(this.handle, frontleft, frontright, center, lfe, surroundleft, surroundright, backleft, backright);
        }
        public RESULT setMixLevelsInput(float[] levels, int numlevels)
        {
            return FMOD5_Channel_SetMixLevelsInput(this.handle, levels, numlevels);
        }
        public RESULT setMixMatrix(float[] matrix, int outchannels, int inchannels, int inchannel_hop = 0)
        {
            return FMOD5_Channel_SetMixMatrix(this.handle, matrix, outchannels, inchannels, inchannel_hop);
        }
        public RESULT getMixMatrix(float[] matrix, out int outchannels, out int inchannels, int inchannel_hop = 0)
        {
            return FMOD5_Channel_GetMixMatrix(this.handle, matrix, out outchannels, out inchannels, inchannel_hop);
        }

        // Clock based functionality.
        public RESULT getDSPClock(out ulong dspclock, out ulong parentclock)
        {
            return FMOD5_Channel_GetDSPClock(this.handle, out dspclock, out parentclock);
        }
        public RESULT setDelay(ulong dspclock_start, ulong dspclock_end, bool stopchannels = true)
        {
            return FMOD5_Channel_SetDelay(this.handle, dspclock_start, dspclock_end, stopchannels);
        }
        public RESULT getDelay(out ulong dspclock_start, out ulong dspclock_end)
        {
            return FMOD5_Channel_GetDelay(this.handle, out dspclock_start, out dspclock_end, IntPtr.Zero);
        }
        public RESULT getDelay(out ulong dspclock_start, out ulong dspclock_end, out bool stopchannels)
        {
            return FMOD5_Channel_GetDelay(this.handle, out dspclock_start, out dspclock_end, out stopchannels);
        }
        public RESULT addFadePoint(ulong dspclock, float volume)
        {
            return FMOD5_Channel_AddFadePoint(this.handle, dspclock, volume);
        }
        public RESULT setFadePointRamp(ulong dspclock, float volume)
        {
            return FMOD5_Channel_SetFadePointRamp(this.handle, dspclock, volume);
        }
        public RESULT removeFadePoints(ulong dspclock_start, ulong dspclock_end)
        {
            return FMOD5_Channel_RemoveFadePoints(this.handle, dspclock_start, dspclock_end);
        }
        public RESULT getFadePoints(ref uint numpoints, ulong[] point_dspclock, float[] point_volume)
        {
            return FMOD5_Channel_GetFadePoints(this.handle, ref numpoints, point_dspclock, point_volume);
        }

        // DSP effects.
        public RESULT getDSP(int index, out DSP dsp)
        {
            return FMOD5_Channel_GetDSP(this.handle, index, out dsp.handle);
        }
        public RESULT addDSP(int index, DSP dsp)
        {
            return FMOD5_Channel_AddDSP(this.handle, index, dsp.handle);
        }
        public RESULT removeDSP(DSP dsp)
        {
            return FMOD5_Channel_RemoveDSP(this.handle, dsp.handle);
        }
        public RESULT getNumDSPs(out int numdsps)
        {
            return FMOD5_Channel_GetNumDSPs(this.handle, out numdsps);
        }
        public RESULT setDSPIndex(DSP dsp, int index)
        {
            return FMOD5_Channel_SetDSPIndex(this.handle, dsp.handle, index);
        }
        public RESULT getDSPIndex(DSP dsp, out int index)
        {
            return FMOD5_Channel_GetDSPIndex(this.handle, dsp.handle, out index);
        }

        // 3D functionality.
        public RESULT set3DAttributes(ref VECTOR pos, ref VECTOR vel)
        {
            return FMOD5_Channel_Set3DAttributes(this.handle, ref pos, ref vel);
        }
        public RESULT get3DAttributes(out VECTOR pos, out VECTOR vel)
        {
            return FMOD5_Channel_Get3DAttributes(this.handle, out pos, out vel);
        }
        public RESULT set3DMinMaxDistance(float mindistance, float maxdistance)
        {
            return FMOD5_Channel_Set3DMinMaxDistance(this.handle, mindistance, maxdistance);
        }
        public RESULT get3DMinMaxDistance(out float mindistance, out float maxdistance)
        {
            return FMOD5_Channel_Get3DMinMaxDistance(this.handle, out mindistance, out maxdistance);
        }
        public RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
        {
            return FMOD5_Channel_Set3DConeSettings(this.handle, insideconeangle, outsideconeangle, outsidevolume);
        }
        public RESULT get3DConeSettings(out float insideconeangle, out float outsideconeangle, out float outsidevolume)
        {
            return FMOD5_Channel_Get3DConeSettings(this.handle, out insideconeangle, out outsideconeangle, out outsidevolume);
        }
        public RESULT set3DConeOrientation(ref VECTOR orientation)
        {
            return FMOD5_Channel_Set3DConeOrientation(this.handle, ref orientation);
        }
        public RESULT get3DConeOrientation(out VECTOR orientation)
        {
            return FMOD5_Channel_Get3DConeOrientation(this.handle, out orientation);
        }
        public RESULT set3DCustomRolloff(ref VECTOR points, int numpoints)
        {
            return FMOD5_Channel_Set3DCustomRolloff(this.handle, ref points, numpoints);
        }
        public RESULT get3DCustomRolloff(out IntPtr points, out int numpoints)
        {
            return FMOD5_Channel_Get3DCustomRolloff(this.handle, out points, out numpoints);
        }
        public RESULT set3DOcclusion(float directocclusion, float reverbocclusion)
        {
            return FMOD5_Channel_Set3DOcclusion(this.handle, directocclusion, reverbocclusion);
        }
        public RESULT get3DOcclusion(out float directocclusion, out float reverbocclusion)
        {
            return FMOD5_Channel_Get3DOcclusion(this.handle, out directocclusion, out reverbocclusion);
        }
        public RESULT set3DSpread(float angle)
        {
            return FMOD5_Channel_Set3DSpread(this.handle, angle);
        }
        public RESULT get3DSpread(out float angle)
        {
            return FMOD5_Channel_Get3DSpread(this.handle, out angle);
        }
        public RESULT set3DLevel(float level)
        {
            return FMOD5_Channel_Set3DLevel(this.handle, level);
        }
        public RESULT get3DLevel(out float level)
        {
            return FMOD5_Channel_Get3DLevel(this.handle, out level);
        }
        public RESULT set3DDopplerLevel(float level)
        {
            return FMOD5_Channel_Set3DDopplerLevel(this.handle, level);
        }
        public RESULT get3DDopplerLevel(out float level)
        {
            return FMOD5_Channel_Get3DDopplerLevel(this.handle, out level);
        }
        public RESULT set3DDistanceFilter(bool custom, float customLevel, float centerFreq)
        {
            return FMOD5_Channel_Set3DDistanceFilter(this.handle, custom, customLevel, centerFreq);
        }
        public RESULT get3DDistanceFilter(out bool custom, out float customLevel, out float centerFreq)
        {
            return FMOD5_Channel_Get3DDistanceFilter(this.handle, out custom, out customLevel, out centerFreq);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_Channel_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_Channel_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetFrequency         (IntPtr channel, float frequency);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetFrequency         (IntPtr channel, out float frequency);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetPriority          (IntPtr channel, int priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetPriority          (IntPtr channel, out int priority);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetPosition          (IntPtr channel, uint position, TIMEUNIT postype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetPosition          (IntPtr channel, out uint position, TIMEUNIT postype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetChannelGroup      (IntPtr channel, IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetChannelGroup      (IntPtr channel, out IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetLoopCount         (IntPtr channel, int loopcount);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetLoopCount         (IntPtr channel, out int loopcount);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetLoopPoints        (IntPtr channel, uint  loopstart, TIMEUNIT loopstarttype, uint  loopend, TIMEUNIT loopendtype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetLoopPoints        (IntPtr channel, out uint loopstart, TIMEUNIT loopstarttype, out uint loopend, TIMEUNIT loopendtype);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_IsVirtual            (IntPtr channel, out bool isvirtual);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetCurrentSound      (IntPtr channel, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetIndex             (IntPtr channel, out int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetSystemObject      (IntPtr channel, out IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Stop                 (IntPtr channel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetPaused            (IntPtr channel, bool paused);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetPaused            (IntPtr channel, out bool paused);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetVolume            (IntPtr channel, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetVolume            (IntPtr channel, out float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetVolumeRamp        (IntPtr channel, bool ramp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetVolumeRamp        (IntPtr channel, out bool ramp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetAudibility        (IntPtr channel, out float audibility);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetPitch             (IntPtr channel, float pitch);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetPitch             (IntPtr channel, out float pitch);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetMute              (IntPtr channel, bool mute);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetMute              (IntPtr channel, out bool mute);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetReverbProperties  (IntPtr channel, int instance, float wet);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetReverbProperties  (IntPtr channel, int instance, out float wet);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetLowPassGain       (IntPtr channel, float gain);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetLowPassGain       (IntPtr channel, out float gain);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetMode              (IntPtr channel, MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetMode              (IntPtr channel, out MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetCallback          (IntPtr channel, CHANNELCONTROL_CALLBACK callback);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_IsPlaying            (IntPtr channel, out bool isplaying);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetPan               (IntPtr channel, float pan);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetMixLevelsOutput   (IntPtr channel, float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetMixLevelsInput    (IntPtr channel, float[] levels, int numlevels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetMixMatrix         (IntPtr channel, float[] matrix, int outchannels, int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetMixMatrix         (IntPtr channel, float[] matrix, out int outchannels, out int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetDSPClock          (IntPtr channel, out ulong dspclock, out ulong parentclock);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetDelay             (IntPtr channel, ulong dspclock_start, ulong dspclock_end, bool stopchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetDelay             (IntPtr channel, out ulong dspclock_start, out ulong dspclock_end, IntPtr zero);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetDelay             (IntPtr channel, out ulong dspclock_start, out ulong dspclock_end, out bool stopchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_AddFadePoint         (IntPtr channel, ulong dspclock, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetFadePointRamp     (IntPtr channel, ulong dspclock, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_RemoveFadePoints     (IntPtr channel, ulong dspclock_start, ulong dspclock_end);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetFadePoints        (IntPtr channel, ref uint numpoints, ulong[] point_dspclock, float[] point_volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetDSP               (IntPtr channel, int index, out IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_AddDSP               (IntPtr channel, int index, IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_RemoveDSP            (IntPtr channel, IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetNumDSPs           (IntPtr channel, out int numdsps);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetDSPIndex          (IntPtr channel, IntPtr dsp, int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetDSPIndex          (IntPtr channel, IntPtr dsp, out int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DAttributes      (IntPtr channel, ref VECTOR pos, ref VECTOR vel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DAttributes      (IntPtr channel, out VECTOR pos, out VECTOR vel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DMinMaxDistance  (IntPtr channel, float mindistance, float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DMinMaxDistance  (IntPtr channel, out float mindistance, out float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DConeSettings    (IntPtr channel, float insideconeangle, float outsideconeangle, float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DConeSettings    (IntPtr channel, out float insideconeangle, out float outsideconeangle, out float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DConeOrientation (IntPtr channel, ref VECTOR orientation);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DConeOrientation (IntPtr channel, out VECTOR orientation);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DCustomRolloff   (IntPtr channel, ref VECTOR points, int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DCustomRolloff   (IntPtr channel, out IntPtr points, out int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DOcclusion       (IntPtr channel, float directocclusion, float reverbocclusion);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DOcclusion       (IntPtr channel, out float directocclusion, out float reverbocclusion);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DSpread          (IntPtr channel, float angle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DSpread          (IntPtr channel, out float angle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DLevel           (IntPtr channel, float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DLevel           (IntPtr channel, out float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DDopplerLevel    (IntPtr channel, float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DDopplerLevel    (IntPtr channel, out float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Set3DDistanceFilter  (IntPtr channel, bool custom, float customLevel, float centerFreq);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_Get3DDistanceFilter  (IntPtr channel, out bool custom, out float customLevel, out float centerFreq);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_SetUserData          (IntPtr channel, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Channel_GetUserData          (IntPtr channel, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'ChannelGroup' API
    */
    public struct ChannelGroup : IChannelControl
    {
        public RESULT release()
        {
            return FMOD5_ChannelGroup_Release(this.handle);
        }

        // Nested channel groups.
        public RESULT addGroup(ChannelGroup group, bool propagatedspclock = true)
        {
            return FMOD5_ChannelGroup_AddGroup(this.handle, group.handle, propagatedspclock, IntPtr.Zero);
        }
        public RESULT addGroup(ChannelGroup group, bool propagatedspclock, out DSPConnection connection)
        {
            return FMOD5_ChannelGroup_AddGroup(this.handle, group.handle, propagatedspclock, out connection.handle);
        }
        public RESULT getNumGroups(out int numgroups)
        {
            return FMOD5_ChannelGroup_GetNumGroups(this.handle, out numgroups);
        }
        public RESULT getGroup(int index, out ChannelGroup group)
        {
            return FMOD5_ChannelGroup_GetGroup(this.handle, index, out group.handle);
        }
        public RESULT getParentGroup(out ChannelGroup group)
        {
            return FMOD5_ChannelGroup_GetParentGroup(this.handle, out group.handle);
        }

        // Information only functions.
        public RESULT getName(out string name, int namelen)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_ChannelGroup_GetName(this.handle, stringMem, namelen);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getNumChannels(out int numchannels)
        {
            return FMOD5_ChannelGroup_GetNumChannels(this.handle, out numchannels);
        }
        public RESULT getChannel(int index, out Channel channel)
        {
            return FMOD5_ChannelGroup_GetChannel(this.handle, index, out channel.handle);
        }

        public RESULT getSystemObject(out System system)
        {
            return FMOD5_ChannelGroup_GetSystemObject(this.handle, out system.handle);
        }

        // General control functionality for Channels and ChannelGroups.
        public RESULT stop()
        {
            return FMOD5_ChannelGroup_Stop(this.handle);
        }
        public RESULT setPaused(bool paused)
        {
            return FMOD5_ChannelGroup_SetPaused(this.handle, paused);
        }
        public RESULT getPaused(out bool paused)
        {
            return FMOD5_ChannelGroup_GetPaused(this.handle, out paused);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD5_ChannelGroup_SetVolume(this.handle, volume);
        }
        public RESULT getVolume(out float volume)
        {
            return FMOD5_ChannelGroup_GetVolume(this.handle, out volume);
        }
        public RESULT setVolumeRamp(bool ramp)
        {
            return FMOD5_ChannelGroup_SetVolumeRamp(this.handle, ramp);
        }
        public RESULT getVolumeRamp(out bool ramp)
        {
            return FMOD5_ChannelGroup_GetVolumeRamp(this.handle, out ramp);
        }
        public RESULT getAudibility(out float audibility)
        {
            return FMOD5_ChannelGroup_GetAudibility(this.handle, out audibility);
        }
        public RESULT setPitch(float pitch)
        {
            return FMOD5_ChannelGroup_SetPitch(this.handle, pitch);
        }
        public RESULT getPitch(out float pitch)
        {
            return FMOD5_ChannelGroup_GetPitch(this.handle, out pitch);
        }
        public RESULT setMute(bool mute)
        {
            return FMOD5_ChannelGroup_SetMute(this.handle, mute);
        }
        public RESULT getMute(out bool mute)
        {
            return FMOD5_ChannelGroup_GetMute(this.handle, out mute);
        }
        public RESULT setReverbProperties(int instance, float wet)
        {
            return FMOD5_ChannelGroup_SetReverbProperties(this.handle, instance, wet);
        }
        public RESULT getReverbProperties(int instance, out float wet)
        {
            return FMOD5_ChannelGroup_GetReverbProperties(this.handle, instance, out wet);
        }
        public RESULT setLowPassGain(float gain)
        {
            return FMOD5_ChannelGroup_SetLowPassGain(this.handle, gain);
        }
        public RESULT getLowPassGain(out float gain)
        {
            return FMOD5_ChannelGroup_GetLowPassGain(this.handle, out gain);
        }
        public RESULT setMode(MODE mode)
        {
            return FMOD5_ChannelGroup_SetMode(this.handle, mode);
        }
        public RESULT getMode(out MODE mode)
        {
            return FMOD5_ChannelGroup_GetMode(this.handle, out mode);
        }
        public RESULT setCallback(CHANNELCONTROL_CALLBACK callback)
        {
            return FMOD5_ChannelGroup_SetCallback(this.handle, callback);
        }
        public RESULT isPlaying(out bool isplaying)
        {
            return FMOD5_ChannelGroup_IsPlaying(this.handle, out isplaying);
        }

        // Panning and level adjustment.
        public RESULT setPan(float pan)
        {
            return FMOD5_ChannelGroup_SetPan(this.handle, pan);
        }
        public RESULT setMixLevelsOutput(float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright)
        {
            return FMOD5_ChannelGroup_SetMixLevelsOutput(this.handle, frontleft, frontright, center, lfe, surroundleft, surroundright, backleft, backright);
        }
        public RESULT setMixLevelsInput(float[] levels, int numlevels)
        {
            return FMOD5_ChannelGroup_SetMixLevelsInput(this.handle, levels, numlevels);
        }
        public RESULT setMixMatrix(float[] matrix, int outchannels, int inchannels, int inchannel_hop)
        {
            return FMOD5_ChannelGroup_SetMixMatrix(this.handle, matrix, outchannels, inchannels, inchannel_hop);
        }
        public RESULT getMixMatrix(float[] matrix, out int outchannels, out int inchannels, int inchannel_hop)
        {
            return FMOD5_ChannelGroup_GetMixMatrix(this.handle, matrix, out outchannels, out inchannels, inchannel_hop);
        }

        // Clock based functionality.
        public RESULT getDSPClock(out ulong dspclock, out ulong parentclock)
        {
            return FMOD5_ChannelGroup_GetDSPClock(this.handle, out dspclock, out parentclock);
        }
        public RESULT setDelay(ulong dspclock_start, ulong dspclock_end, bool stopchannels)
        {
            return FMOD5_ChannelGroup_SetDelay(this.handle, dspclock_start, dspclock_end, stopchannels);
        }
        public RESULT getDelay(out ulong dspclock_start, out ulong dspclock_end)
        {
            return FMOD5_ChannelGroup_GetDelay(this.handle, out dspclock_start, out dspclock_end, IntPtr.Zero);
        }
        public RESULT getDelay(out ulong dspclock_start, out ulong dspclock_end, out bool stopchannels)
        {
            return FMOD5_ChannelGroup_GetDelay(this.handle, out dspclock_start, out dspclock_end, out stopchannels);
        }
        public RESULT addFadePoint(ulong dspclock, float volume)
        {
            return FMOD5_ChannelGroup_AddFadePoint(this.handle, dspclock, volume);
        }
        public RESULT setFadePointRamp(ulong dspclock, float volume)
        {
            return FMOD5_ChannelGroup_SetFadePointRamp(this.handle, dspclock, volume);
        }
        public RESULT removeFadePoints(ulong dspclock_start, ulong dspclock_end)
        {
            return FMOD5_ChannelGroup_RemoveFadePoints(this.handle, dspclock_start, dspclock_end);
        }
        public RESULT getFadePoints(ref uint numpoints, ulong[] point_dspclock, float[] point_volume)
        {
            return FMOD5_ChannelGroup_GetFadePoints(this.handle, ref numpoints, point_dspclock, point_volume);
        }

        // DSP effects.
        public RESULT getDSP(int index, out DSP dsp)
        {
            return FMOD5_ChannelGroup_GetDSP(this.handle, index, out dsp.handle);
        }
        public RESULT addDSP(int index, DSP dsp)
        {
            return FMOD5_ChannelGroup_AddDSP(this.handle, index, dsp.handle);
        }
        public RESULT removeDSP(DSP dsp)
        {
            return FMOD5_ChannelGroup_RemoveDSP(this.handle, dsp.handle);
        }
        public RESULT getNumDSPs(out int numdsps)
        {
            return FMOD5_ChannelGroup_GetNumDSPs(this.handle, out numdsps);
        }
        public RESULT setDSPIndex(DSP dsp, int index)
        {
            return FMOD5_ChannelGroup_SetDSPIndex(this.handle, dsp.handle, index);
        }
        public RESULT getDSPIndex(DSP dsp, out int index)
        {
            return FMOD5_ChannelGroup_GetDSPIndex(this.handle, dsp.handle, out index);
        }

        // 3D functionality.
        public RESULT set3DAttributes(ref VECTOR pos, ref VECTOR vel)
        {
            return FMOD5_ChannelGroup_Set3DAttributes(this.handle, ref pos, ref vel);
        }
        public RESULT get3DAttributes(out VECTOR pos, out VECTOR vel)
        {
            return FMOD5_ChannelGroup_Get3DAttributes(this.handle, out pos, out vel);
        }
        public RESULT set3DMinMaxDistance(float mindistance, float maxdistance)
        {
            return FMOD5_ChannelGroup_Set3DMinMaxDistance(this.handle, mindistance, maxdistance);
        }
        public RESULT get3DMinMaxDistance(out float mindistance, out float maxdistance)
        {
            return FMOD5_ChannelGroup_Get3DMinMaxDistance(this.handle, out mindistance, out maxdistance);
        }
        public RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
        {
            return FMOD5_ChannelGroup_Set3DConeSettings(this.handle, insideconeangle, outsideconeangle, outsidevolume);
        }
        public RESULT get3DConeSettings(out float insideconeangle, out float outsideconeangle, out float outsidevolume)
        {
            return FMOD5_ChannelGroup_Get3DConeSettings(this.handle, out insideconeangle, out outsideconeangle, out outsidevolume);
        }
        public RESULT set3DConeOrientation(ref VECTOR orientation)
        {
            return FMOD5_ChannelGroup_Set3DConeOrientation(this.handle, ref orientation);
        }
        public RESULT get3DConeOrientation(out VECTOR orientation)
        {
            return FMOD5_ChannelGroup_Get3DConeOrientation(this.handle, out orientation);
        }
        public RESULT set3DCustomRolloff(ref VECTOR points, int numpoints)
        {
            return FMOD5_ChannelGroup_Set3DCustomRolloff(this.handle, ref points, numpoints);
        }
        public RESULT get3DCustomRolloff(out IntPtr points, out int numpoints)
        {
            return FMOD5_ChannelGroup_Get3DCustomRolloff(this.handle, out points, out numpoints);
        }
        public RESULT set3DOcclusion(float directocclusion, float reverbocclusion)
        {
            return FMOD5_ChannelGroup_Set3DOcclusion(this.handle, directocclusion, reverbocclusion);
        }
        public RESULT get3DOcclusion(out float directocclusion, out float reverbocclusion)
        {
            return FMOD5_ChannelGroup_Get3DOcclusion(this.handle, out directocclusion, out reverbocclusion);
        }
        public RESULT set3DSpread(float angle)
        {
            return FMOD5_ChannelGroup_Set3DSpread(this.handle, angle);
        }
        public RESULT get3DSpread(out float angle)
        {
            return FMOD5_ChannelGroup_Get3DSpread(this.handle, out angle);
        }
        public RESULT set3DLevel(float level)
        {
            return FMOD5_ChannelGroup_Set3DLevel(this.handle, level);
        }
        public RESULT get3DLevel(out float level)
        {
            return FMOD5_ChannelGroup_Get3DLevel(this.handle, out level);
        }
        public RESULT set3DDopplerLevel(float level)
        {
            return FMOD5_ChannelGroup_Set3DDopplerLevel(this.handle, level);
        }
        public RESULT get3DDopplerLevel(out float level)
        {
            return FMOD5_ChannelGroup_Get3DDopplerLevel(this.handle, out level);
        }
        public RESULT set3DDistanceFilter(bool custom, float customLevel, float centerFreq)
        {
            return FMOD5_ChannelGroup_Set3DDistanceFilter(this.handle, custom, customLevel, centerFreq);
        }
        public RESULT get3DDistanceFilter(out bool custom, out float customLevel, out float centerFreq)
        {
            return FMOD5_ChannelGroup_Get3DDistanceFilter(this.handle, out custom, out customLevel, out centerFreq);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_ChannelGroup_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_ChannelGroup_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Release             (IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_AddGroup            (IntPtr channelgroup, IntPtr group, bool propagatedspclock, IntPtr zero);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_AddGroup            (IntPtr channelgroup, IntPtr group, bool propagatedspclock, out IntPtr connection);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetNumGroups        (IntPtr channelgroup, out int numgroups);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetGroup            (IntPtr channelgroup, int index, out IntPtr group);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetParentGroup      (IntPtr channelgroup, out IntPtr group);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetName             (IntPtr channelgroup, IntPtr name, int namelen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetNumChannels      (IntPtr channelgroup, out int numchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetChannel          (IntPtr channelgroup, int index, out IntPtr channel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetSystemObject     (IntPtr channelgroup, out IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Stop                (IntPtr channelgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetPaused           (IntPtr channelgroup, bool paused);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetPaused           (IntPtr channelgroup, out bool paused);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetVolume           (IntPtr channelgroup, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetVolume           (IntPtr channelgroup, out float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetVolumeRamp       (IntPtr channelgroup, bool ramp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetVolumeRamp       (IntPtr channelgroup, out bool ramp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetAudibility       (IntPtr channelgroup, out float audibility);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetPitch            (IntPtr channelgroup, float pitch);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetPitch            (IntPtr channelgroup, out float pitch);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetMute             (IntPtr channelgroup, bool mute);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetMute             (IntPtr channelgroup, out bool mute);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetReverbProperties (IntPtr channelgroup, int instance, float wet);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetReverbProperties (IntPtr channelgroup, int instance, out float wet);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetLowPassGain      (IntPtr channelgroup, float gain);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetLowPassGain      (IntPtr channelgroup, out float gain);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetMode             (IntPtr channelgroup, MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetMode             (IntPtr channelgroup, out MODE mode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetCallback         (IntPtr channelgroup, CHANNELCONTROL_CALLBACK callback);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_IsPlaying           (IntPtr channelgroup, out bool isplaying);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetPan              (IntPtr channelgroup, float pan);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetMixLevelsOutput  (IntPtr channelgroup, float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetMixLevelsInput   (IntPtr channelgroup, float[] levels, int numlevels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetMixMatrix        (IntPtr channelgroup, float[] matrix, int outchannels, int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetMixMatrix        (IntPtr channelgroup, float[] matrix, out int outchannels, out int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetDSPClock         (IntPtr channelgroup, out ulong dspclock, out ulong parentclock);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetDelay            (IntPtr channelgroup, ulong dspclock_start, ulong dspclock_end, bool stopchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetDelay            (IntPtr channelgroup, out ulong dspclock_start, out ulong dspclock_end, IntPtr zero);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetDelay            (IntPtr channelgroup, out ulong dspclock_start, out ulong dspclock_end, out bool stopchannels);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_AddFadePoint        (IntPtr channelgroup, ulong dspclock, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetFadePointRamp    (IntPtr channelgroup, ulong dspclock, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_RemoveFadePoints    (IntPtr channelgroup, ulong dspclock_start, ulong dspclock_end);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetFadePoints       (IntPtr channelgroup, ref uint numpoints, ulong[] point_dspclock, float[] point_volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetDSP              (IntPtr channelgroup, int index, out IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_AddDSP              (IntPtr channelgroup, int index, IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_RemoveDSP           (IntPtr channelgroup, IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetNumDSPs          (IntPtr channelgroup, out int numdsps);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetDSPIndex         (IntPtr channelgroup, IntPtr dsp, int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetDSPIndex         (IntPtr channelgroup, IntPtr dsp, out int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DAttributes     (IntPtr channelgroup, ref VECTOR pos, ref VECTOR vel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DAttributes     (IntPtr channelgroup, out VECTOR pos, out VECTOR vel);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DMinMaxDistance (IntPtr channelgroup, float mindistance, float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DMinMaxDistance (IntPtr channelgroup, out float mindistance, out float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DConeSettings   (IntPtr channelgroup, float insideconeangle, float outsideconeangle, float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DConeSettings   (IntPtr channelgroup, out float insideconeangle, out float outsideconeangle, out float outsidevolume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DConeOrientation(IntPtr channelgroup, ref VECTOR orientation);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DConeOrientation(IntPtr channelgroup, out VECTOR orientation);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DCustomRolloff  (IntPtr channelgroup, ref VECTOR points, int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DCustomRolloff  (IntPtr channelgroup, out IntPtr points, out int numpoints);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DOcclusion      (IntPtr channelgroup, float directocclusion, float reverbocclusion);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DOcclusion      (IntPtr channelgroup, out float directocclusion, out float reverbocclusion);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DSpread         (IntPtr channelgroup, float angle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DSpread         (IntPtr channelgroup, out float angle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DLevel          (IntPtr channelgroup, float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DLevel          (IntPtr channelgroup, out float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DDopplerLevel   (IntPtr channelgroup, float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DDopplerLevel   (IntPtr channelgroup, out float level);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Set3DDistanceFilter (IntPtr channelgroup, bool custom, float customLevel, float centerFreq);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_Get3DDistanceFilter (IntPtr channelgroup, out bool custom, out float customLevel, out float centerFreq);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_SetUserData         (IntPtr channelgroup, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_ChannelGroup_GetUserData         (IntPtr channelgroup, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'SoundGroup' API
    */
    public struct SoundGroup
    {
        public RESULT release()
        {
            return FMOD5_SoundGroup_Release(this.handle);
        }

        public RESULT getSystemObject(out System system)
        {
            return FMOD5_SoundGroup_GetSystemObject(this.handle, out system.handle);
        }

        // SoundGroup control functions.
        public RESULT setMaxAudible(int maxaudible)
        {
            return FMOD5_SoundGroup_SetMaxAudible(this.handle, maxaudible);
        }
        public RESULT getMaxAudible(out int maxaudible)
        {
            return FMOD5_SoundGroup_GetMaxAudible(this.handle, out maxaudible);
        }
        public RESULT setMaxAudibleBehavior(SOUNDGROUP_BEHAVIOR behavior)
        {
            return FMOD5_SoundGroup_SetMaxAudibleBehavior(this.handle, behavior);
        }
        public RESULT getMaxAudibleBehavior(out SOUNDGROUP_BEHAVIOR behavior)
        {
            return FMOD5_SoundGroup_GetMaxAudibleBehavior(this.handle, out behavior);
        }
        public RESULT setMuteFadeSpeed(float speed)
        {
            return FMOD5_SoundGroup_SetMuteFadeSpeed(this.handle, speed);
        }
        public RESULT getMuteFadeSpeed(out float speed)
        {
            return FMOD5_SoundGroup_GetMuteFadeSpeed(this.handle, out speed);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD5_SoundGroup_SetVolume(this.handle, volume);
        }
        public RESULT getVolume(out float volume)
        {
            return FMOD5_SoundGroup_GetVolume(this.handle, out volume);
        }
        public RESULT stop()
        {
            return FMOD5_SoundGroup_Stop(this.handle);
        }

        // Information only functions.
        public RESULT getName(out string name, int namelen)
        {
            IntPtr stringMem = Marshal.AllocHGlobal(namelen);

            RESULT result = FMOD5_SoundGroup_GetName(this.handle, stringMem, namelen);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(stringMem);
            }
            Marshal.FreeHGlobal(stringMem);

            return result;
        }
        public RESULT getNumSounds(out int numsounds)
        {
            return FMOD5_SoundGroup_GetNumSounds(this.handle, out numsounds);
        }
        public RESULT getSound(int index, out Sound sound)
        {
            return FMOD5_SoundGroup_GetSound(this.handle, index, out sound.handle);
        }
        public RESULT getNumPlaying(out int numplaying)
        {
            return FMOD5_SoundGroup_GetNumPlaying(this.handle, out numplaying);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_SoundGroup_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_SoundGroup_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_Release               (IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetSystemObject       (IntPtr soundgroup, out IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_SetMaxAudible         (IntPtr soundgroup, int maxaudible);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetMaxAudible         (IntPtr soundgroup, out int maxaudible);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_SetMaxAudibleBehavior (IntPtr soundgroup, SOUNDGROUP_BEHAVIOR behavior);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetMaxAudibleBehavior (IntPtr soundgroup, out SOUNDGROUP_BEHAVIOR behavior);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_SetMuteFadeSpeed      (IntPtr soundgroup, float speed);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetMuteFadeSpeed      (IntPtr soundgroup, out float speed);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_SetVolume             (IntPtr soundgroup, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetVolume             (IntPtr soundgroup, out float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_Stop                  (IntPtr soundgroup);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetName               (IntPtr soundgroup, IntPtr name, int namelen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetNumSounds          (IntPtr soundgroup, out int numsounds);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetSound              (IntPtr soundgroup, int index, out IntPtr sound);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetNumPlaying         (IntPtr soundgroup, out int numplaying);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_SetUserData           (IntPtr soundgroup, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_SoundGroup_GetUserData           (IntPtr soundgroup, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'DSP' API
    */
    public struct DSP
    {
        public RESULT release()
        {
            return FMOD5_DSP_Release(this.handle);
        }
        public RESULT getSystemObject(out System system)
        {
            return FMOD5_DSP_GetSystemObject(this.handle, out system.handle);
        }

        // Connection / disconnection / input and output enumeration.
        public RESULT addInput(DSP input)
        {
            return FMOD5_DSP_AddInput(this.handle, input.handle, IntPtr.Zero, DSPCONNECTION_TYPE.STANDARD);
        }
        public RESULT addInput(DSP input, out DSPConnection connection, DSPCONNECTION_TYPE type = DSPCONNECTION_TYPE.STANDARD)
        {
            return FMOD5_DSP_AddInput(this.handle, input.handle, out connection.handle, type);
        }
        public RESULT disconnectFrom(DSP target, DSPConnection connection)
        {
            return FMOD5_DSP_DisconnectFrom(this.handle, target.handle, connection.handle);
        }
        public RESULT disconnectAll(bool inputs, bool outputs)
        {
            return FMOD5_DSP_DisconnectAll(this.handle, inputs, outputs);
        }
        public RESULT getNumInputs(out int numinputs)
        {
            return FMOD5_DSP_GetNumInputs(this.handle, out numinputs);
        }
        public RESULT getNumOutputs(out int numoutputs)
        {
            return FMOD5_DSP_GetNumOutputs(this.handle, out numoutputs);
        }
        public RESULT getInput(int index, out DSP input, out DSPConnection inputconnection)
        {
            return FMOD5_DSP_GetInput(this.handle, index, out input.handle, out inputconnection.handle);
        }
        public RESULT getOutput(int index, out DSP output, out DSPConnection outputconnection)
        {
            return FMOD5_DSP_GetOutput(this.handle, index, out output.handle, out outputconnection.handle);
        }

        // DSP unit control.
        public RESULT setActive(bool active)
        {
            return FMOD5_DSP_SetActive(this.handle, active);
        }
        public RESULT getActive(out bool active)
        {
            return FMOD5_DSP_GetActive(this.handle, out active);
        }
        public RESULT setBypass(bool bypass)
        {
            return FMOD5_DSP_SetBypass(this.handle, bypass);
        }
        public RESULT getBypass(out bool bypass)
        {
            return FMOD5_DSP_GetBypass(this.handle, out bypass);
        }
        public RESULT setWetDryMix(float prewet, float postwet, float dry)
        {
            return FMOD5_DSP_SetWetDryMix(this.handle, prewet, postwet, dry);
        }
        public RESULT getWetDryMix(out float prewet, out float postwet, out float dry)
        {
            return FMOD5_DSP_GetWetDryMix(this.handle, out prewet, out postwet, out dry);
        }
        public RESULT setChannelFormat(CHANNELMASK channelmask, int numchannels, SPEAKERMODE source_speakermode)
        {
            return FMOD5_DSP_SetChannelFormat(this.handle, channelmask, numchannels, source_speakermode);
        }
        public RESULT getChannelFormat(out CHANNELMASK channelmask, out int numchannels, out SPEAKERMODE source_speakermode)
        {
            return FMOD5_DSP_GetChannelFormat(this.handle, out channelmask, out numchannels, out source_speakermode);
        }
        public RESULT getOutputChannelFormat(CHANNELMASK inmask, int inchannels, SPEAKERMODE inspeakermode, out CHANNELMASK outmask, out int outchannels, out SPEAKERMODE outspeakermode)
        {
            return FMOD5_DSP_GetOutputChannelFormat(this.handle, inmask, inchannels, inspeakermode, out outmask, out outchannels, out outspeakermode);
        }
        public RESULT reset()
        {
            return FMOD5_DSP_Reset(this.handle);
        }

        // DSP parameter control.
        public RESULT setParameterFloat(int index, float value)
        {
            return FMOD5_DSP_SetParameterFloat(this.handle, index, value);
        }
        public RESULT setParameterInt(int index, int value)
        {
            return FMOD5_DSP_SetParameterInt(this.handle, index, value);
        }
        public RESULT setParameterBool(int index, bool value)
        {
            return FMOD5_DSP_SetParameterBool(this.handle, index, value);
        }
        public RESULT setParameterData(int index, byte[] data)
        {
            return FMOD5_DSP_SetParameterData(this.handle, index, Marshal.UnsafeAddrOfPinnedArrayElement(data, 0), (uint)data.Length);
        }
        public RESULT getParameterFloat(int index, out float value)
        {
            return FMOD5_DSP_GetParameterFloat(this.handle, index, out value, IntPtr.Zero, 0);
        }
        public RESULT getParameterInt(int index, out int value)
        {
            return FMOD5_DSP_GetParameterInt(this.handle, index, out value, IntPtr.Zero, 0);
        }
        public RESULT getParameterBool(int index, out bool value)
        {
            return FMOD5_DSP_GetParameterBool(this.handle, index, out value, IntPtr.Zero, 0);
        }
        public RESULT getParameterData(int index, out IntPtr data, out uint length)
        {
            return FMOD5_DSP_GetParameterData(this.handle, index, out data, out length, IntPtr.Zero, 0);
        }
        public RESULT getNumParameters(out int numparams)
        {
            return FMOD5_DSP_GetNumParameters(this.handle, out numparams);
        }
        public RESULT getParameterInfo(int index, out DSP_PARAMETER_DESC desc)
        {
            IntPtr descPtr;
            RESULT result = FMOD5_DSP_GetParameterInfo(this.handle, index, out descPtr);
            #if (UNITY_2017_4_OR_NEWER) && !NET_4_6
            desc = (DSP_PARAMETER_DESC)Marshal.PtrToStructure(descPtr, typeof(DSP_PARAMETER_DESC));
            #else
            desc = Marshal.PtrToStructure<DSP_PARAMETER_DESC>(descPtr);
            #endif // (UNITY_2017_4_OR_NEWER) && !NET_4_6
            return result;
        }
        public RESULT getDataParameterIndex(int datatype, out int index)
        {
            return FMOD5_DSP_GetDataParameterIndex(this.handle, datatype, out index);
        }
        public RESULT showConfigDialog(IntPtr hwnd, bool show)
        {
            return FMOD5_DSP_ShowConfigDialog(this.handle, hwnd, show);
        }

        //  DSP attributes.
        public RESULT getInfo(out string name, out uint version, out int channels, out int configwidth, out int configheight)
        {
            IntPtr nameMem = Marshal.AllocHGlobal(32);

            RESULT result = FMOD5_DSP_GetInfo(this.handle, nameMem, out version, out channels, out configwidth, out configheight);
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                name = encoder.stringFromNative(nameMem);
            }
            Marshal.FreeHGlobal(nameMem);
            return result;
        }
        public RESULT getInfo(out uint version, out int channels, out int configwidth, out int configheight)
        {
            return FMOD5_DSP_GetInfo(this.handle, IntPtr.Zero, out version, out channels, out configwidth, out configheight); ;
        }
        public RESULT getType(out DSP_TYPE type)
        {
            return FMOD5_DSP_GetType(this.handle, out type);
        }
        public RESULT getIdle(out bool idle)
        {
            return FMOD5_DSP_GetIdle(this.handle, out idle);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_DSP_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_DSP_GetUserData(this.handle, out userdata);
        }

        // Metering.
        public RESULT setMeteringEnabled(bool inputEnabled, bool outputEnabled)
        {
            return FMOD5_DSP_SetMeteringEnabled(this.handle, inputEnabled, outputEnabled);
        }
        public RESULT getMeteringEnabled(out bool inputEnabled, out bool outputEnabled)
        {
            return FMOD5_DSP_GetMeteringEnabled(this.handle, out inputEnabled, out outputEnabled);
        }

        public RESULT getMeteringInfo(IntPtr zero, out DSP_METERING_INFO outputInfo)
        {
            return FMOD5_DSP_GetMeteringInfo(this.handle, zero, out outputInfo);
        }
        public RESULT getMeteringInfo(out DSP_METERING_INFO inputInfo, IntPtr zero)
        {
            return FMOD5_DSP_GetMeteringInfo(this.handle, out inputInfo, zero);
        }
        public RESULT getMeteringInfo(out DSP_METERING_INFO inputInfo, out DSP_METERING_INFO outputInfo)
        {
            return FMOD5_DSP_GetMeteringInfo(this.handle, out inputInfo, out outputInfo);
        }

        public RESULT getCPUUsage(out uint exclusive, out uint inclusive)
        {
            return FMOD5_DSP_GetCPUUsage(this.handle, out exclusive, out inclusive);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_Release                   (IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetSystemObject           (IntPtr dsp, out IntPtr system);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_AddInput                  (IntPtr dsp, IntPtr input, IntPtr zero, DSPCONNECTION_TYPE type);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_AddInput                  (IntPtr dsp, IntPtr input, out IntPtr connection, DSPCONNECTION_TYPE type);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_DisconnectFrom            (IntPtr dsp, IntPtr target, IntPtr connection);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_DisconnectAll             (IntPtr dsp, bool inputs, bool outputs);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetNumInputs              (IntPtr dsp, out int numinputs);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetNumOutputs             (IntPtr dsp, out int numoutputs);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetInput                  (IntPtr dsp, int index, out IntPtr input, out IntPtr inputconnection);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetOutput                 (IntPtr dsp, int index, out IntPtr output, out IntPtr outputconnection);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetActive                 (IntPtr dsp, bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetActive                 (IntPtr dsp, out bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetBypass                 (IntPtr dsp, bool bypass);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetBypass                 (IntPtr dsp, out bool bypass);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetWetDryMix              (IntPtr dsp, float prewet, float postwet, float dry);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetWetDryMix              (IntPtr dsp, out float prewet, out float postwet, out float dry);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetChannelFormat          (IntPtr dsp, CHANNELMASK channelmask, int numchannels, SPEAKERMODE source_speakermode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetChannelFormat          (IntPtr dsp, out CHANNELMASK channelmask, out int numchannels, out SPEAKERMODE source_speakermode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetOutputChannelFormat    (IntPtr dsp, CHANNELMASK inmask, int inchannels, SPEAKERMODE inspeakermode, out CHANNELMASK outmask, out int outchannels, out SPEAKERMODE outspeakermode);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_Reset                     (IntPtr dsp);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetParameterFloat         (IntPtr dsp, int index, float value);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetParameterInt           (IntPtr dsp, int index, int value);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetParameterBool          (IntPtr dsp, int index, bool value);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetParameterData          (IntPtr dsp, int index, IntPtr data, uint length);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetParameterFloat         (IntPtr dsp, int index, out float value, IntPtr valuestr, int valuestrlen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetParameterInt           (IntPtr dsp, int index, out int value, IntPtr valuestr, int valuestrlen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetParameterBool          (IntPtr dsp, int index, out bool value, IntPtr valuestr, int valuestrlen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetParameterData          (IntPtr dsp, int index, out IntPtr data, out uint length, IntPtr valuestr, int valuestrlen);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetNumParameters          (IntPtr dsp, out int numparams);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetParameterInfo          (IntPtr dsp, int index, out IntPtr desc);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetDataParameterIndex     (IntPtr dsp, int datatype, out int index);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_ShowConfigDialog          (IntPtr dsp, IntPtr hwnd, bool show);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetInfo                   (IntPtr dsp, IntPtr name, out uint version, out int channels, out int configwidth, out int configheight);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetType                   (IntPtr dsp, out DSP_TYPE type);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetIdle                   (IntPtr dsp, out bool idle);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_SetUserData               (IntPtr dsp, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSP_GetUserData               (IntPtr dsp, out IntPtr userdata);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_SetMeteringEnabled         (IntPtr dsp, bool inputEnabled, bool outputEnabled);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_GetMeteringEnabled         (IntPtr dsp, out bool inputEnabled, out bool outputEnabled);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_GetMeteringInfo            (IntPtr dsp, IntPtr zero, out DSP_METERING_INFO outputInfo);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_GetMeteringInfo            (IntPtr dsp, out DSP_METERING_INFO inputInfo, IntPtr zero);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_GetMeteringInfo            (IntPtr dsp, out DSP_METERING_INFO inputInfo, out DSP_METERING_INFO outputInfo);
        [DllImport(VERSION.dll)]
        public static extern RESULT FMOD5_DSP_GetCPUUsage                (IntPtr dsp, out uint exclusive, out uint inclusive);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'DSPConnection' API
    */
    public struct DSPConnection
    {
        public RESULT getInput(out DSP input)
        {
            return FMOD5_DSPConnection_GetInput(this.handle, out input.handle);
        }
        public RESULT getOutput(out DSP output)
        {
            return FMOD5_DSPConnection_GetOutput(this.handle, out output.handle);
        }
        public RESULT setMix(float volume)
        {
            return FMOD5_DSPConnection_SetMix(this.handle, volume);
        }
        public RESULT getMix(out float volume)
        {
            return FMOD5_DSPConnection_GetMix(this.handle, out volume);
        }
        public RESULT setMixMatrix(float[] matrix, int outchannels, int inchannels, int inchannel_hop = 0)
        {
            return FMOD5_DSPConnection_SetMixMatrix(this.handle, matrix, outchannels, inchannels, inchannel_hop);
        }
        public RESULT getMixMatrix(float[] matrix, out int outchannels, out int inchannels, int inchannel_hop = 0)
        {
            return FMOD5_DSPConnection_GetMixMatrix(this.handle, matrix, out outchannels, out inchannels, inchannel_hop);
        }
        public RESULT getType(out DSPCONNECTION_TYPE type)
        {
            return FMOD5_DSPConnection_GetType(this.handle, out type);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_DSPConnection_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_DSPConnection_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetInput        (IntPtr dspconnection, out IntPtr input);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetOutput       (IntPtr dspconnection, out IntPtr output);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_SetMix          (IntPtr dspconnection, float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetMix          (IntPtr dspconnection, out float volume);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_SetMixMatrix    (IntPtr dspconnection, float[] matrix, int outchannels, int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetMixMatrix    (IntPtr dspconnection, float[] matrix, out int outchannels, out int inchannels, int inchannel_hop);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetType         (IntPtr dspconnection, out DSPCONNECTION_TYPE type);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_SetUserData     (IntPtr dspconnection, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_DSPConnection_GetUserData     (IntPtr dspconnection, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'Geometry' API
    */
    public struct Geometry
    {
        public RESULT release()
        {
            return FMOD5_Geometry_Release(this.handle);
        }

        // Polygon manipulation.
        public RESULT addPolygon(float directocclusion, float reverbocclusion, bool doublesided, int numvertices, VECTOR[] vertices, out int polygonindex)
        {
            return FMOD5_Geometry_AddPolygon(this.handle, directocclusion, reverbocclusion, doublesided, numvertices, vertices, out polygonindex);
        }
        public RESULT getNumPolygons(out int numpolygons)
        {
            return FMOD5_Geometry_GetNumPolygons(this.handle, out numpolygons);
        }
        public RESULT getMaxPolygons(out int maxpolygons, out int maxvertices)
        {
            return FMOD5_Geometry_GetMaxPolygons(this.handle, out maxpolygons, out maxvertices);
        }
        public RESULT getPolygonNumVertices(int index, out int numvertices)
        {
            return FMOD5_Geometry_GetPolygonNumVertices(this.handle, index, out numvertices);
        }
        public RESULT setPolygonVertex(int index, int vertexindex, ref VECTOR vertex)
        {
            return FMOD5_Geometry_SetPolygonVertex(this.handle, index, vertexindex, ref vertex);
        }
        public RESULT getPolygonVertex(int index, int vertexindex, out VECTOR vertex)
        {
            return FMOD5_Geometry_GetPolygonVertex(this.handle, index, vertexindex, out vertex);
        }
        public RESULT setPolygonAttributes(int index, float directocclusion, float reverbocclusion, bool doublesided)
        {
            return FMOD5_Geometry_SetPolygonAttributes(this.handle, index, directocclusion, reverbocclusion, doublesided);
        }
        public RESULT getPolygonAttributes(int index, out float directocclusion, out float reverbocclusion, out bool doublesided)
        {
            return FMOD5_Geometry_GetPolygonAttributes(this.handle, index, out directocclusion, out reverbocclusion, out doublesided);
        }

        // Object manipulation.
        public RESULT setActive(bool active)
        {
            return FMOD5_Geometry_SetActive(this.handle, active);
        }
        public RESULT getActive(out bool active)
        {
            return FMOD5_Geometry_GetActive(this.handle, out active);
        }
        public RESULT setRotation(ref VECTOR forward, ref VECTOR up)
        {
            return FMOD5_Geometry_SetRotation(this.handle, ref forward, ref up);
        }
        public RESULT getRotation(out VECTOR forward, out VECTOR up)
        {
            return FMOD5_Geometry_GetRotation(this.handle, out forward, out up);
        }
        public RESULT setPosition(ref VECTOR position)
        {
            return FMOD5_Geometry_SetPosition(this.handle, ref position);
        }
        public RESULT getPosition(out VECTOR position)
        {
            return FMOD5_Geometry_GetPosition(this.handle, out position);
        }
        public RESULT setScale(ref VECTOR scale)
        {
            return FMOD5_Geometry_SetScale(this.handle, ref scale);
        }
        public RESULT getScale(out VECTOR scale)
        {
            return FMOD5_Geometry_GetScale(this.handle, out scale);
        }
        public RESULT save(IntPtr data, out int datasize)
        {
            return FMOD5_Geometry_Save(this.handle, data, out datasize);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_Geometry_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_Geometry_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_Release              (IntPtr geometry);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_AddPolygon           (IntPtr geometry, float directocclusion, float reverbocclusion, bool doublesided, int numvertices, VECTOR[] vertices, out int polygonindex);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetNumPolygons       (IntPtr geometry, out int numpolygons);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetMaxPolygons       (IntPtr geometry, out int maxpolygons, out int maxvertices);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetPolygonNumVertices(IntPtr geometry, int index, out int numvertices);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetPolygonVertex     (IntPtr geometry, int index, int vertexindex, ref VECTOR vertex);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetPolygonVertex     (IntPtr geometry, int index, int vertexindex, out VECTOR vertex);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetPolygonAttributes (IntPtr geometry, int index, float directocclusion, float reverbocclusion, bool doublesided);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetPolygonAttributes (IntPtr geometry, int index, out float directocclusion, out float reverbocclusion, out bool doublesided);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetActive            (IntPtr geometry, bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetActive            (IntPtr geometry, out bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetRotation          (IntPtr geometry, ref VECTOR forward, ref VECTOR up);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetRotation          (IntPtr geometry, out VECTOR forward, out VECTOR up);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetPosition          (IntPtr geometry, ref VECTOR position);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetPosition          (IntPtr geometry, out VECTOR position);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetScale             (IntPtr geometry, ref VECTOR scale);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetScale             (IntPtr geometry, out VECTOR scale);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_Save                 (IntPtr geometry, IntPtr data, out int datasize);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_SetUserData          (IntPtr geometry, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Geometry_GetUserData          (IntPtr geometry, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    /*
        'Reverb3D' API
    */
    public struct Reverb3D
    {
        public RESULT release()
        {
            return FMOD5_Reverb3D_Release(this.handle);
        }

        // Reverb manipulation.
        public RESULT set3DAttributes(ref VECTOR position, float mindistance, float maxdistance)
        {
            return FMOD5_Reverb3D_Set3DAttributes(this.handle, ref position, mindistance, maxdistance);
        }
        public RESULT get3DAttributes(ref VECTOR position, ref float mindistance, ref float maxdistance)
        {
            return FMOD5_Reverb3D_Get3DAttributes(this.handle, ref position, ref mindistance, ref maxdistance);
        }
        public RESULT setProperties(ref REVERB_PROPERTIES properties)
        {
            return FMOD5_Reverb3D_SetProperties(this.handle, ref properties);
        }
        public RESULT getProperties(ref REVERB_PROPERTIES properties)
        {
            return FMOD5_Reverb3D_GetProperties(this.handle, ref properties);
        }
        public RESULT setActive(bool active)
        {
            return FMOD5_Reverb3D_SetActive(this.handle, active);
        }
        public RESULT getActive(out bool active)
        {
            return FMOD5_Reverb3D_GetActive(this.handle, out active);
        }

        // Userdata set/get.
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD5_Reverb3D_SetUserData(this.handle, userdata);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD5_Reverb3D_GetUserData(this.handle, out userdata);
        }

        #region importfunctions
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_Release             (IntPtr reverb3d);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_Set3DAttributes     (IntPtr reverb3d, ref VECTOR position, float mindistance, float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_Get3DAttributes     (IntPtr reverb3d, ref VECTOR position, ref float mindistance, ref float maxdistance);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_SetProperties       (IntPtr reverb3d, ref REVERB_PROPERTIES properties);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_GetProperties       (IntPtr reverb3d, ref REVERB_PROPERTIES properties);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_SetActive           (IntPtr reverb3d, bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_GetActive           (IntPtr reverb3d, out bool active);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_SetUserData         (IntPtr reverb3d, IntPtr userdata);
        [DllImport(VERSION.dll)]
        private static extern RESULT FMOD5_Reverb3D_GetUserData         (IntPtr reverb3d, out IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        #endregion
    }

    #region String Helper Functions
    [StructLayout(LayoutKind.Sequential)]
    public struct StringWrapper
    {
        IntPtr nativeUtf8Ptr;

        public static implicit operator string(StringWrapper fstring)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return encoder.stringFromNative(fstring.nativeUtf8Ptr);
            }
        }
    }

    static class StringHelper
    {
        public class ThreadSafeEncoding : IDisposable
        {
            UTF8Encoding encoding = new UTF8Encoding();
            byte[] encodedBuffer = new byte[128];
            char[] decodedBuffer = new char[128];
            bool inUse;
            GCHandle gcHandle;

            public bool InUse()    { return inUse; }
            public void SetInUse() { inUse = true; }

            private int roundUpPowerTwo(int number)
            {
                int newNumber = 1;
                while (newNumber <= number)
                {
                    newNumber *= 2;
                }

                return newNumber;
            }

            public byte[] byteFromStringUTF8(string s)
            {
                if (s == null)
                {
                    return null;
                }

                int maximumLength = encoding.GetMaxByteCount(s.Length) + 1; // +1 for null terminator
                if (maximumLength > encodedBuffer.Length)
                {
                    int encodedLength = encoding.GetByteCount(s) + 1; // +1 for null terminator
                    if (encodedLength > encodedBuffer.Length)
                    {
                        encodedBuffer = new byte[roundUpPowerTwo(encodedLength)];
                    }
                }

                int byteCount = encoding.GetBytes(s, 0, s.Length, encodedBuffer, 0);
                encodedBuffer[byteCount] = 0; // Apply null terminator

                return encodedBuffer;
            }

            public IntPtr intptrFromStringUTF8(string s)
            {
                if (s == null)
                {
                    return IntPtr.Zero;
                }

                gcHandle = GCHandle.Alloc(byteFromStringUTF8(s), GCHandleType.Pinned);
                return gcHandle.AddrOfPinnedObject();
            }

            public string stringFromNative(IntPtr nativePtr)
            {
                if (nativePtr == IntPtr.Zero)
                {
                    return "";
                }

                int nativeLen = 0;
                while (Marshal.ReadByte(nativePtr, nativeLen) != 0)
                {
                    nativeLen++;
                }

                if (nativeLen == 0)
                {
                    return "";
                }

                if (nativeLen > encodedBuffer.Length)
                {
                    encodedBuffer = new byte[roundUpPowerTwo(nativeLen)];
                }

                Marshal.Copy(nativePtr, encodedBuffer, 0, nativeLen);

                int maximumLength = encoding.GetMaxCharCount(nativeLen);
                if (maximumLength > decodedBuffer.Length)
                {
                    int decodedLength = encoding.GetCharCount(encodedBuffer, 0, nativeLen);
                    if (decodedLength > decodedBuffer.Length)
                    {
                        decodedBuffer = new char[roundUpPowerTwo(decodedLength)];
                    }
                }

                int charCount = encoding.GetChars(encodedBuffer, 0, nativeLen, decodedBuffer, 0);

                return new String(decodedBuffer, 0, charCount);
            }

            public void Dispose()
            {
                if (gcHandle.IsAllocated)
                {
                    gcHandle.Free();
                }
                lock (encoders)
                {
                    inUse = false;
                }
            }
        }

        static List<ThreadSafeEncoding> encoders = new List<ThreadSafeEncoding>(1);

        public static ThreadSafeEncoding GetFreeHelper()
        {
            lock (encoders)
            {
                ThreadSafeEncoding helper = null;
                // Search for not in use helper
                for (int i = 0; i < encoders.Count; i++)
                {
                    if (!encoders[i].InUse())
                    {
                        helper = encoders[i];
                        break;
                    }
                }
                // Otherwise create another helper
                if (helper == null)
                {
                    helper = new ThreadSafeEncoding();
                    encoders.Add(helper);
                }
                helper.SetInUse();
                return helper;
            }
        }
    }

    #endregion
}
