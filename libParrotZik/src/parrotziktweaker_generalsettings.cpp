#include "parrotziktweaker.h"

void ParrotZikTweeker::setFriendlyName(const QString &name)
{
    if (name != m_friendlyName) {

        QString command("GET /api/bluetooth/friendlyname/set?arg=");
        command = command.append(name);

        this->sendMessage(command);

        updateFriendlyName();
    }
}

void ParrotZikTweeker::setAccountUsername(const QString &username)
{
    if (username != m_accountUsername) {

        QString command("GET /api/account/username/set?arg=");
        command = command.append(username);

        this->sendMessage(command);

        updateAccountUsername();
    }
}

void ParrotZikTweeker::setAutoConnection(QString status)
{
    QString command("SET /api/system/auto_connection/enabled/set?arg=");
    command = command.append(status);

    this->sendMessage(command);
}

void ParrotZikTweeker::getAutoConnection()
{
    QString command("GET /api/system/auto_connection/enabled/get");

    this->sendMessage(command);
}


void ParrotZikTweeker::setTtsEnabled(const bool &enable)
{
    if(enable != m_ttsEnabled){
        QString command;

        if(enable){
            command = QString("GET /api/software/tts/enable");
        }else{
            command = QString("GET /api/software/tts/disable");
        }

        m_ttsEnabled = enable;
        emit ttsEnabledChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setFlightModeEnabled(const bool &enable)
{
    if(enable != m_flightModeEnabled){
        QString command;

        if(enable){
            command = QString("GET /api/flight_mode/enable");
        }else{
            command = QString("GET /api/flight_mode/disable");
        }

        m_flightModeEnabled = enable;
        emit flightModeEnabledChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setHeadDetectionEnabled(const bool &enable)
{
    if(enable != m_headDetectionEnabled){
        QString command;

        if(enable){
            command = QString("GET /api/system/head_detection/enabled/set?arg=true");
        }else{
            command = QString("GET /api/system/head_detection/enabled/set?arg=false");
        }

        m_headDetectionEnabled = enable;
        emit headDetectionEnabledChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setAutoConnectionEnabled(const bool &enable)
{
    if(enable != m_autoConnectionEnabled){
        QString command;

        if(enable){
            command = QString("GET /api/system/auto_connection/enabled/set?arg=true");
        }else{
            command = QString("GET /api/system/auto_connection/enabled/set?arg=false");
        }

        m_autoConnectionEnabled = enable;
        emit autoConnectionEnabledChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setA2dpDelay(const int &delay)
{
    if(delay != m_a2dpDelay){
        QString command;
        command = QString("GET /api/audio/delay/set?arg=%1").arg(delay);

        m_a2dpDelay = delay;
        emit a2dpDelayChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setAutoPowerOff(const int &value)
{
    if (value != m_autoPowerOff) {

        QString command;
        command = QString("GET /api/system/auto_power_off/set?arg=%1").arg(value);

        m_autoPowerOff = value;
        emit autoPowerOffChanged();

        this->sendMessage(command);
    }
}
