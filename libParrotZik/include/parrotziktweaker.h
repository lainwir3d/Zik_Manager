#ifndef PARROTZIKTWEEKER_H
#define PARROTZIKTWEEKER_H

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothServiceDiscoveryAgent>
#include <QtBluetooth/QBluetoothServiceInfo>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothSocket>
#include <QtBluetooth/QBluetoothAddress>
#include <QString>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QThread>
#include <QList>
#include <QPoint>
#include <QTimer>
#include <QVariantMap>
#include <QQueue>
#include <QTime>
#include <QMutex>
#include <QtQml/QtQml>

#if defined(Q_OS_ANDROID)
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif


#include "gain.h"
#include "presetmodel.h"

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusArgument>
#include <QDBusError>
#endif

#if defined(Q_OS_LINUX) or defined(Q_OS_MACOS)
#include <unistd.h>
#endif

#if defined(Q_OS_ANDROID)
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif

struct DevStruct {
    QString address;
    QString name;
    QStringList uuids;
};
typedef DevStruct DevStruct;
class PresetModel;

#if (QT_VERSION < QT_VERSION_CHECK(5, 2, 0)) && (!QTBLUETOOTH_BACKPORT)
using namespace QtBluetooth;
#endif

class ParrotZikTweeker: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString model READ model NOTIFY modelChanged)

    Q_PROPERTY(QString battery READ battery WRITE setBattery NOTIFY batteryChanged)
    Q_PROPERTY(QString batteryState READ batteryState NOTIFY batteryStateChanged)

    Q_PROPERTY(QString friendlyName READ friendlyName WRITE setFriendlyName NOTIFY friendlyNameChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QString picVersion READ picVersion NOTIFY picVersionChanged)
    Q_PROPERTY(QString accountUsername READ accountUsername WRITE setAccountUsername NOTIFY accountUsernameChanged)
    Q_PROPERTY(QString ttsLang READ ttsLang NOTIFY ttsLangChanged)
    Q_PROPERTY(QString ttsLangHumanReadable READ ttsLangHumanReadable NOTIFY ttsLangHumanReadableChanged)


    Q_PROPERTY(QString concertHall READ concertHall WRITE setConcertHall NOTIFY concertHallChanged)
    Q_PROPERTY(QString roomSize READ roomSize WRITE setRoomSize NOTIFY roomSizeChanged)
    Q_PROPERTY(QString spatializerAngle READ spatializerAngle WRITE setSpatializerAngle NOTIFY spatializerAngleChanged)
    Q_PROPERTY(bool surroundMode READ surroundMode NOTIFY surroundModeChanged)


    Q_PROPERTY(QString equalizer READ equalizer WRITE setEqualizer NOTIFY equalizerChanged)
    Q_PROPERTY(QPoint thumb READ thumb WRITE setThumb NOTIFY thumbChanged)
    Q_PROPERTY(int oldPresetId READ oldPresetId WRITE setOldPresetId NOTIFY oldPresetIdChanged)

    Q_PROPERTY(QString anc READ anc WRITE setAnc NOTIFY ancChanged)
    Q_PROPERTY(QString ncEnabled READ ncEnabled WRITE setNcEnabled NOTIFY ncEnabledChanged)
    Q_PROPERTY(bool autoNC READ autoNC WRITE setAutoNC NOTIFY autoNCChanged)
    Q_PROPERTY(QString ncType READ ncType WRITE setNcType NOTIFY ncTypeChanged)
    Q_PROPERTY(QString ncValue READ ncValue WRITE setNcValue NOTIFY ncValueChanged)
    Q_PROPERTY(QString phoneModeNcType READ phoneModeNcType WRITE setPhoneModeNcType NOTIFY phoneModeNcTypeChanged)
    Q_PROPERTY(QString phoneModeNcValue READ phoneModeNcValue WRITE setPhoneModeNcValue NOTIFY phoneModeNcValueChanged)

    Q_PROPERTY(QString smartAudioTune READ smartAudioTune WRITE setSmartAudioTune NOTIFY smartAudioTuneChanged)
    Q_PROPERTY(QString presetBypass READ presetBypass WRITE setPresetBypass NOTIFY presetBypassChanged)
    Q_PROPERTY(QString presetCounter READ presetCounter WRITE setPresetCounter NOTIFY presetCounterChanged)
    Q_PROPERTY(int currentId READ currentId NOTIFY currentIdChanged)

    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(QString song READ song NOTIFY songChanged)
    Q_PROPERTY(QString genre READ genre NOTIFY genreChanged)
    Q_PROPERTY(QString artUrl READ artUrl NOTIFY artUrlChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)


    Q_PROPERTY(QString externalNoise READ externalNoise NOTIFY externalNoiseChanged)
    Q_PROPERTY(QString internalNoise READ internalNoise NOTIFY internalNoiseChanged)
    Q_PROPERTY(int noiseReductionLevel READ noiseReductionLevel NOTIFY noiseReductionLevelChanged)

    Q_PROPERTY(bool ttsEnabled READ ttsEnabled WRITE setTtsEnabled NOTIFY ttsEnabledChanged)
    Q_PROPERTY(bool flightModeEnabled READ flightModeEnabled WRITE setFlightModeEnabled NOTIFY flightModeEnabledChanged)
    Q_PROPERTY(bool headDetectionEnabled READ headDetectionEnabled WRITE setHeadDetectionEnabled NOTIFY headDetectionEnabledChanged)
    Q_PROPERTY(bool autoConnectionEnabled READ autoConnectionEnabled WRITE setAutoConnectionEnabled NOTIFY autoConnectionEnabledChanged)


    Q_PROPERTY(bool producerMode READ producerMode WRITE setProducerMode NOTIFY producerModeChanged)


    Q_PROPERTY(bool demoMode READ demoMode WRITE setDemoMode NOTIFY demoModeChanged)
    Q_PROPERTY(bool bluetoothOn READ bluetoothOn WRITE setBluetoothOn NOTIFY bluetoothOnChanged)
    Q_PROPERTY(bool viewVisible READ viewVisible NOTIFY viewVisibleChanged)
    Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)

    Q_PROPERTY(Capabilities capabilities READ capabilities NOTIFY capabilitiesChanged)

    Q_PROPERTY(int a2dpDelay READ a2dpDelay WRITE setA2dpDelay NOTIFY a2dpDelayChanged)

    Q_PROPERTY(QString color READ color NOTIFY colorChanged)
    Q_PROPERTY(QString texture READ texture NOTIFY textureChanged)

    Q_PROPERTY(int autoPowerOff READ autoPowerOff WRITE setAutoPowerOff NOTIFY autoPowerOffChanged)

    Q_ENUMS(Capabilities)
public:
    ParrotZikTweeker();
    ParrotZikTweeker(QString btAddr);
    ~ParrotZikTweeker();

    Q_INVOKABLE void connectTo(QString btaddr);

    enum Capability {
        NoCapability = 0,
        Anc = 1,
        ConcertHall = 2,
        Linein = 4,
        BasicSettings = 8,
        ThumbEqualizer = 16,
        Telephony = 32,
        TTS = 64,
        StreetMode = 128,
        FlightMode = 256,
        Preset = 512,
        PresetMultiDevice = 1024,
        AutoNc = 2048,
        NcDuringCall = 4096,
        USBAudio = 8192,
        USBAudio51 = 16384,
        ConcertHallSurround = 32768,
        BluetoothDelay = 65536,
        BluetoothMultiPoint = 131072,
        BatteryNotification = 524288
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    static void declareQML() {
        qmlRegisterType<ParrotZikTweeker>("ZikManager", 1, 0, "Zik");
    }


    Capabilities capabilities() const {return m_capabilities;}
    void updateCapabilities() { sendMessage("GET /api/features/get"); }
    void setCapabilities(const Capabilities &capa);

    bool alwaysVisible() const {return m_alwaysVisible;}
    void setAlwaysVisible(const bool &always);

    QString model() const {return m_model;}

    void setAppliVersion(QString version);

    PresetModel * presetModel;

    QStringList discoveredMprisPlayer();

    QString battery() const {return m_battery;}
    QString batteryState() const {return m_batteryState;}
    void setBattery(const QString &a);

    int autoPowerOff() const {return m_autoPowerOff;}
    void updateAutoPowerOff() { sendMessage("GET /api/system/auto_power_off/get"); }
    void setAutoPowerOff(const int &value);

    QString friendlyName() const {return m_friendlyName;}
    void updateFriendlyName() { sendMessage("GET /api/bluetooth/friendlyname/get"); }
    Q_INVOKABLE void setFriendlyName(const QString &a);

    QString accountUsername() const {return m_accountUsername;}
    void updateAccountUsername() { sendMessage("GET /api/account/username/get"); }
    Q_INVOKABLE void setAccountUsername(const QString &username);

    QString picVersion() const {return m_picVersion;}
    QString version() const {return m_version;}
    QString ttsLang() const {return m_ttsLang;}
    QString ttsLangHumanReadable() const {return m_ttsLangHumanReadable;}
    Q_INVOKABLE void updateVersion() { sendMessage("GET /api/software/version/get"); }
    void setVersion(const QString &v);

    QString concertHall() const {return m_concertHall;}
    Q_INVOKABLE void updateConcertHall() { sendMessage("GET /api/audio/sound_effect/enabled/get"); }
    void setConcertHall(const QString &status);

    QString roomSize() const {return m_roomSize;}
    Q_INVOKABLE void updateRoomSize() { sendMessage("GET /api/audio/sound_effect/room_size/get"); }
    void setRoomSize(const QString &s);

    QString spatializerAngle() const {return m_spatializerAngle;}
    Q_INVOKABLE void updateSpatializerAngle() { sendMessage("GET /api/audio/sound_effect/angle/get"); }
    void setSpatializerAngle(const QString &s);

    bool surroundMode() const {return m_surroundMode;}
    void updateSurroundMode() { sendMessage("GET /api/audio/sound_effect/mode/get"); }

    void updateAllSpatialisation() { sendMessage("GET /api/audio/sound_effect/get"); }

    QString anc() const {return m_anc;}
    Q_INVOKABLE void updateAnc() { sendMessage("GET /api/audio/noise_cancellation/enabled/get"); }
    void setAnc(const QString &status);

    Q_INVOKABLE void getAoc() { sendMessage("GET /api/audio/noise_control/get"); }
    Q_INVOKABLE void setAoc(QString type, int value);

    QString ncEnabled() const {return m_ncEnabled;}
    Q_INVOKABLE void updateNcEnabled() { sendMessage("GET /api/audio/noise_control/enabled/get"); }
    void setNcEnabled(const QString &enable);

    bool autoNC() const {return m_autoNC;}
    void setAutoNC(const bool &enable);

    QString ncType() const {return m_ncType;}
    void updateNcType() { sendMessage("GET /api/audio/noise_control/get"); }
    void setNcType(const QString &type);

    QString ncValue() const {return m_ncValue;}
    void setNcValue(const QString &value);

    QString phoneModeNcType() const {return m_phoneModeNcType;}
    void updatePhoneModeNc() { sendMessage("GET /api/audio/noise_control/phone_mode/get"); }
    void setPhoneModeNcType(const QString &type);

    QString phoneModeNcValue() const {return m_phoneModeNcValue;}
    void setPhoneModeNcValue(const QString &value);
    Q_INVOKABLE void setPhoneModeNc(QString type, int value);

    QString equalizer() const {return m_equalizer;}
    Q_INVOKABLE void updateEqualizer() { sendMessage("GET /api/audio/equalizer/enabled/get"); }
    void setEqualizer(const QString &status);

    QPoint thumb() const {return m_thumb;}
    Q_INVOKABLE void updateThumb() { sendMessage("GET /api/audio/thumb_equalizer/value/get"); }
    void setThumb(const QPoint &p);

    // zik 1 equalizer presets
    int oldPresetId() const {return m_oldPresetId;}
    void updateOldPresetId() { sendMessage("GET /api/audio/equalizer/preset_id/get"); }
    void setOldPresetId(const int &i);

    QString smartAudioTune() const {return m_smartAudioTune;}
    Q_INVOKABLE void updateSmartAudioTune() { sendMessage("GET /api/audio/smart_audio_tune/get"); }
    void setSmartAudioTune(const QString &enable);

    QString presetBypass() const {return m_presetBypass;}
    Q_INVOKABLE void updatePresetBypass() { sendMessage("GET /api/audio/preset/bypass/get"); }
    void setPresetBypass(const QString &enable);

    QString presetCounter() const {return m_presetCounter;}
    Q_INVOKABLE void updatePresetCounter() { sendMessage("GET /api/audio/preset/counter/get"); }
    void setPresetCounter(const QString &counter);

    int currentId() const {return m_currentId;}
    Q_INVOKABLE void updateCurrentPreset() { sendMessage("GET /api/audio/preset/current/get"); }



    QString artist() const {return m_artist;}
    QString album() const {return m_album;}
    QString song() const {return m_song;}
    QString genre() const {return m_genre;}
    QString artUrl() const {return mpris_artUrl;}
    bool playing() const {return m_playing;}
    QString source() const {return m_source;}
    Q_INVOKABLE void updateMetadata() { sendMessage("GET /api/audio/track/metadata/get"); }
    Q_INVOKABLE void updateSource() { sendMessage("GET /api/audio/source/get"); }

#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
    void forceMetadata(QString artist, QString album, QString song, QString genre);
#endif

    QString internalNoise() const {return m_internalNoise;}
    QString externalNoise() const {return m_externalNoise;}
    int noiseReductionLevel() const {return m_noiseReductionLevel;}
    Q_INVOKABLE void updateNoise() { sendMessage("GET /api/audio/noise/get"); }


    void startClient(const QBluetoothServiceInfo &remoteService);
    Q_INVOKABLE int isConnected();

    Q_INVOKABLE void setAutoConnection(QString status);
    Q_INVOKABLE void getAutoConnection();

    Q_INVOKABLE void setParametricEqualizer(int freq1, int freq2, int freq3, int freq4, int freq5,
                                            float q1, float q2, float q3, float q4, float q5,
                                            float gain1, float gain2, float gain3, float gain4, float gain5);

    Q_INVOKABLE void savePreset(QString name, QString album, bool linkAlbum, QString artist, bool linkArtist, QString genre, bool linkGenre, QString song, bool linkSong, bool generateId = false);
    Q_INVOKABLE void saveQuickPreset(QString name, QString album, bool linkAlbum, QString artist, bool linkArtist, QString genre, bool linkGenre, QString song, bool linkSong, bool generateId = false);
    Q_INVOKABLE void downloadPreset(QString name, QString album, int linkAlbum, QString artist, int linkArtist, QString genre, int linkGenre, QString song, int linkSong, int spatializerEnabled, int room, int angle, QString equa, int id = -1, int modelNumber = -1);
    Q_INVOKABLE void deletePreset(int id);
    Q_INVOKABLE void enablePreset(int id, bool status);
    Q_INVOKABLE void clearAllPresets();
    void startPresetSynchronization();
    void stopPresetSynchronization();
    Q_INVOKABLE void cancelProducerPreset();


    bool ttsEnabled() const {return m_ttsEnabled;}
    void updateTtsEnabled() { sendMessage("GET /api/software/tts/get"); }
    void setTtsEnabled(const bool &enable);

    bool flightModeEnabled() const {return m_flightModeEnabled;}
    void updateFlightModeEnabled() { sendMessage("GET /api/flight_mode/get"); }
    void setFlightModeEnabled(const bool &enable);

    bool headDetectionEnabled() const { return m_headDetectionEnabled; }
    void updateHeadDetectionEnabled() { sendMessage("GET /api/system/head_detection/enabled/get"); }
    void setHeadDetectionEnabled(const bool &enable);

    bool autoConnectionEnabled() const { return m_autoConnectionEnabled; }
    void updateAutoConnectionEnabled() { sendMessage("GET /api/system/auto_connection/enabled/get"); }
    void setAutoConnectionEnabled(const bool &enable);

    bool producerMode() const {return m_producerMode;}
    void setProducerMode(const bool &enable);

    bool demoMode() const {return m_demoMode;}
    void setDemoMode(const bool &enable);

    bool bluetoothOn() const {return m_bluetoothOn;}
    void setBluetoothOn(const bool &enable);

    bool viewVisible() const {return m_viewVisible;}
    Q_INVOKABLE int generateRandom(int low, int high);

    bool downloadInProgress() const {return m_downloadInProgress;}

    int a2dpDelay() const {return m_a2dpDelay;}
    void updateA2dpDelay() { sendMessage("GET /api/audio/delay/get"); }
    void setA2dpDelay(const int &delay);

    QString color() const { return m_color; }
    void updateColor() { sendMessage("GET /api/system/color/get"); sendMessage("GET /api/system/color/get?version=2"); }

    QString texture() const { return m_texture; }
    void updateTexture() { sendMessage("GET /api/system/texture/get"); }

    Q_INVOKABLE void updateParametricEqualizerValues() { sendMessage("GET /api/audio/param_equalizer/value/get"); }

    void updatePINumber() { sendMessage("GET /api/system/pi/get"); }

public slots:
    void sendMessage(const QString &message, const bool &waitForAnswer = true);
    Q_INVOKABLE void sendPlfFile(QString lang);

    void sendTestEmbedCommand(const QString &command, const bool &waitForAnswer = true);

    void stopClient();
    void socketError(QBluetoothSocket::SocketError error);
    void socketStateChanged(QBluetoothSocket::SocketState state);

    void metadataCallback(QString dbusinterface, QVariantMap map, QStringList l);
    void getMprisMetadata(bool forceUpdate = false);

    void updateBatteryLevel() { sendMessage("GET /api/system/battery/get"); }

    void manageQueue();
	void closeBTConnectionsBeforeQuit();


    Q_INVOKABLE void setApplicationVisible(bool visible);


signals:
    void connected();
    void disconnected();
    void answerReceived();
    void newMessageInQueue();

    void capabilitiesChanged();

    void modelChanged();

    void autoPowerOffChanged();
    void batteryChanged();
    void batteryStateChanged();
    void friendlyNameChanged();
    void accountUsernameChanged();
    void versionChanged();
    void picVersionChanged();
    void ttsLangChanged();
    void ttsLangHumanReadableChanged();

    void concertHallChanged();
    void roomSizeChanged();
    void spatializerAngleChanged();
    void surroundModeChanged();

    void ancChanged();
    void ncEnabledChanged();
    void autoNCChanged();
    void ncTypeChanged();
    void ncValueChanged();
    void phoneModeNcTypeChanged();
    void phoneModeNcValueChanged();

    void equalizerChanged();
    void thumbChanged();
    void oldPresetIdChanged();

    void smartAudioTuneChanged();
    void presetBypassChanged();
    void presetCounterChanged();
    void currentIdChanged();

    void artistChanged();
    void albumChanged();
    void songChanged();
    void genreChanged();
    void artUrlChanged();
    void metadataChanged();
    void playingChanged();
    void sourceChanged();

    void internalNoiseChanged();
    void externalNoiseChanged();
    void noiseReductionLevelChanged();
    void noiseChanged();

    void ttsEnabledChanged();

    void flightModeEnabledChanged();

    void headDetectionEnabledChanged();
    void autoConnectionEnabledChanged();

    void producerModeChanged();

    void demoModeChanged();
    void bluetoothOnChanged();
    void viewVisibleChanged();

    void downloadInProgressChanged();

    void a2dpDelayChanged();

    void colorChanged();
    void textureChanged();

private slots:
    void startAutoConnection();
    void readSocket();
    void socketConnected();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void deviceDiscoveryFinished();
    void serviceDiscovered(const QBluetoothServiceInfo &service);
    void serviceDiscoveryFinished();

    void bluetoothModeChanged(QBluetoothLocalDevice::HostMode);

    void mprisInit();

    void sendPlfPacket();
private:
    void commonInit();

    int connection;
    QBluetoothLocalDevice * localDevice;
    QBluetoothSocket * socket;
    QQueue<QString> command_queue;

    Capabilities m_capabilities;
    void setDefaultCapabilities();
    QList<DevStruct> getKnownDevices();
    #ifdef Q_OS_ANDROID
    bool checkException(const char* method, const QAndroidJniObject* obj);
    #endif


    QString m_color;
    QString m_texture;

    bool m_alwaysVisible;

    int counter_sent;
    int counter_received;
    int counter_timeout;

    QTimer * autoconnection_timer;
    QTimer * battery_timer;
    QTimer * queue_noblock;
    QTimer * answerTimeout_timer;

    QFile * plfFile;
    QTimer * plfPacket_timer;
    QMutex * plfMutex;



#if defined(Q_OS_LINUX) and !defined(Q_OS_ANDROID)
    QDBusInterface * dbus;
    QTimer * metadata_timer;
    QTimer * mpris_timer;
    QString mpris_artist;
    QString mpris_album;
    QString mpris_song;
    QString mpris_genre;
    bool mpris_playing;
#endif
    QString mpris_artUrl;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothServiceDiscoveryAgent  * serviceDiscoveryAgent;
    QEventLoop waitForAnswerLoop;

    QString m_model;

    int m_autoPowerOff;
    QString m_battery;
    QString m_batteryState;
    QString m_friendlyName;
    QString m_accountUsername;
    QString m_version;
    QString m_picVersion;
    QString m_ttsLang;
    QString m_ttsLangHumanReadable;

    QString m_concertHall;
    QString m_roomSize;
    QString m_spatializerAngle;
    bool m_surroundMode;

    QString m_anc;
    QString m_ncEnabled;
    bool m_autoNC;
    QString m_ncType;
    QString m_ncValue;
    QString m_phoneModeNcType;
    QString m_phoneModeNcValue;


    QString m_equalizer;
    QPoint m_thumb;
    int m_oldPresetId;

    QString m_smartAudioTune;
    QString m_presetBypass;
    QString m_presetCounter;
    int m_currentId;

    QString m_artist;
    QString m_album;
    QString m_song;
    QString m_genre;
    bool m_playing;
    QString m_source;

    QString m_internalNoise;
    QString m_externalNoise;
    int m_noiseReductionLevel;

    bool m_ttsEnabled;

    bool m_flightModeEnabled;

    bool m_headDetectionEnabled;
    bool m_autoConnectionEnabled;

    bool m_producerMode;

    bool m_demoMode;
    bool m_bluetoothOn;
    bool m_viewVisible;

    bool m_downloadInProgress;

    int m_a2dpDelay;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ParrotZikTweeker::Capabilities)

#endif // PARROTZIKTWEEKER_H
