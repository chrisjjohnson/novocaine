//
//  Novocaine.h
//
//  Created by Alex Wiltschko on 7/7/10.
//  Copyright 2012 Alex Wiltschko. All rights reserved.
//

#import <CoreFoundation/CoreFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <Accelerate/Accelerate.h>

#if defined __MAC_OS_X_VERSION_MAX_ALLOWED
    #define USING_OSX 
    #include <CoreAudio/CoreAudio.h>
#else
    #define USING_IOS
#endif

#include <Block.h>


static void CheckError(OSStatus error, const char *operation)
{
	if (error == noErr) return;
	
	char str[20];
	// see if it appears to be a 4-char-code
	*(UInt32 *)(str + 1) = CFSwapInt32HostToBig(error);
	if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4])) {
		str[0] = str[5] = '\'';
		str[6] = '\0';
	} else
		// no, format it as an integer
		sprintf(str, "%d", (int)error);
    
	fprintf(stderr, "Error: %s (%s)\n", operation, str);
    
	exit(1);
}


OSStatus inputCallback (void						*inRefCon,
						AudioUnitRenderActionFlags	* ioActionFlags,
						const AudioTimeStamp 		* inTimeStamp,
						UInt32						inOutputBusNumber,
						UInt32						inNumberFrames,
						AudioBufferList				* ioData);

OSStatus renderCallback (void						*inRefCon,
                         AudioUnitRenderActionFlags	* ioActionFlags,
                         const AudioTimeStamp 		* inTimeStamp,
                         UInt32						inOutputBusNumber,
                         UInt32						inNumberFrames,
                         AudioBufferList				* ioData);


#if defined (USING_IOS)
void sessionPropertyListener(void *                  inClientData,
							 AudioSessionPropertyID  inID,
							 UInt32                  inDataSize,
							 const void *            inData);

#endif


void sessionInterruptionListener(void *inClientData, UInt32 inInterruption);

typedef void (^OutputBlock)(float *data, UInt32 numFrames, UInt32 numChannels);
typedef void (^InputBlock)(float *data, UInt32 numFrames, UInt32 numChannels);

#if defined (USING_IOS)
@interface Novocaine : NSObject <UIAlertViewDelegate>
#elif defined (USING_OSX)
@interface Novocaine : NSObject
#endif
{    
	// Audio Handling
	AudioUnit inputUnit;
    AudioUnit outputUnit;
    AudioBufferList *inputBuffer;
    
	// Session Properties
	BOOL inputAvailable;
	NSString *inputRoute;
	UInt32 numInputChannels;
	UInt32 numOutputChannels;
    Float64 samplingRate;
    BOOL isInterleaved;
    UInt32 numBytesPerSample;
	
	// Audio Processing
    OutputBlock outputBlock;
    InputBlock inputBlock;
    
	float *inData;
    float *outData;
	
	BOOL playing;
    // BOOL playThroughEnabled;
	
    
#if defined (USING_OSX)
    AudioDeviceID *deviceIDs;
    NSMutableArray *deviceNames;
    AudioDeviceID defaultInputDeviceID;
    NSString *defaultDeviceName;
#endif
    
}

@property AudioUnit inputUnit;
@property AudioUnit outputUnit;
@property AudioBufferList *inputBuffer;
@property (nonatomic, retain) OutputBlock outputBlock;
@property (nonatomic, retain) InputBlock inputBlock;
@property BOOL inputAvailable;
@property (nonatomic, retain) NSString *inputRoute;
@property UInt32 numInputChannels;
@property UInt32 numOutputChannels;
@property Float64 samplingRate;
@property BOOL isInterleaved;
@property UInt32 numBytesPerSample;

// @property BOOL playThroughEnabled;
@property BOOL playing;
@property float *inData;
@property float *outData;

#if defined (USING_OSX)
@property AudioDeviceID *deviceIDs;
@property (nonatomic, retain) NSMutableArray *deviceNames;
@property  AudioDeviceID defaultInputDeviceID;
@property (nonatomic, retain) NSString *defaultInputDeviceName;
@property  AudioDeviceID defaultOutputDeviceID;
@property (nonatomic, retain) NSString *defaultOutputDeviceName;
- (void)enumerateAudioDevices;
#endif


// Singleton methods
+ (Novocaine *) audioManager;


// Audio Unit methods
- (void)play;
- (void)pause;
- (void)setupAudio;
- (void)ifAudioInputIsAvailableThenSetupAudioSession;

#if defined ( USING_IOS )
- (void)checkSessionProperties;
- (void)checkAudioSource;
#endif


@end