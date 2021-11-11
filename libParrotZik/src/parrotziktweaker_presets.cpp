#include "parrotziktweaker.h"

void ParrotZikTweeker::setProducerMode(const bool &enable)
{
    qDebug() << "pre Setting producer mode to " << enable;
    if(m_producerMode != enable){
        qDebug() << "Setting producer mode to " << enable;
        m_producerMode = enable;
        emit producerModeChanged();
    }
}

void ParrotZikTweeker::savePreset(QString name, QString album, bool linkAlbum, QString artist, bool linkArtist, QString genre, bool linkGenre, QString song, bool linkSong, bool generateId)
{
    QString command;
    int room = roomSize() == "silent" ? 0 : roomSize() == "living" ? 1 : roomSize() == "jazz" ? 2 : 3;


    int modelNumber;

    if(model() == "ZIK1"){
        modelNumber = 1;
    }else if(model() == "ZIK2"){
        modelNumber = 3;
    }else if(model() == "ZIK3"){
        modelNumber = 5;
    }else if(model() == "ZIKSPORT"){
        modelNumber = 4;
    }

    int suggested_id = presetModel->insertPreset(name,
                             concertHall() == "true" ? true : false, room, spatializerAngle().toInt(),
                             linkAlbum ? album : "", linkAlbum, linkArtist ? artist : "", linkArtist,
                             linkGenre ? genre : "", linkGenre, linkSong ? song : "", linkSong, generateId, modelNumber);


    if(!generateId){
        command = QString("GET /api/audio/preset/save?name=%1&album=%2&link_album=%3&artist=%4&link_artist=%5&genre=%6&link_genre=%7&song=%8&link_song=%9").arg(name.replace("&","\\&")).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0);
    }else{
        command = QString("GET /api/audio/preset/save?name=%1&id=%2&album=%3&link_album=%4&artist=%5&link_artist=%6&genre=%7&link_genre=%8&song=%9&link_song=%10").arg(name.replace("&","\\&")).arg(suggested_id).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0);
    }
    qDebug() << "Sending message : " << command;
    this->sendMessage(command);
}

void ParrotZikTweeker::saveQuickPreset(QString name, QString album, bool linkAlbum, QString artist, bool linkArtist, QString genre, bool linkGenre, QString song, bool linkSong, bool generateId)
{
    QString command;
    int room = roomSize() == "silent" ? 0 : roomSize() == "living" ? 1 : roomSize() == "jazz" ? 2 : 3;

    float gain1 = thumbToGain[m_thumb.x()][m_thumb.y()][0];
    float gain2 = thumbToGain[m_thumb.x()][m_thumb.y()][1];
    float gain3 = thumbToGain[m_thumb.x()][m_thumb.y()][2];
    float gain4 = thumbToGain[m_thumb.x()][m_thumb.y()][3];
    float gain5 = thumbToGain[m_thumb.x()][m_thumb.y()][4];

    presetModel->setEquaParameters(70, 381, 960, 2419, 11000,
                                      0.3, 0.8, 1, 0.8, 0.3,
                                      gain1, gain2, gain3, gain4, gain5);

    int modelNumber;
    if(model() == "ZIK1"){
        modelNumber = 1;
    }else if(model() == "ZIK2"){
        modelNumber = 3;
    }else if(model() == "ZIK3"){
        modelNumber = 5;
    }else if(model() == "ZIKSPORT"){
        modelNumber = 4;
    }

    int suggested_id = presetModel->insertPreset(name,
                             concertHall() == "true" ? true : false, room, spatializerAngle().toInt(),
                             linkAlbum ? album : "", linkAlbum, linkArtist ? artist : "", linkArtist,
                             linkGenre ? genre : "", linkGenre, linkSong ? song : "", linkSong, generateId, modelNumber);


    if(!generateId){
        command = QString("GET /api/audio/preset/save?name=%1&album=%2&link_album=%3&artist=%4&link_artist=%5&genre=%6&link_genre=%7&song=%8&link_song=%9").arg(name.replace("&","\\&")).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0);
    }else{
        command = QString("GET /api/audio/preset/save?name=%1&id=%2&album=%3&link_album=%4&artist=%5&link_artist=%6&genre=%7&link_genre=%8&song=%9&link_song=%10").arg(name.replace("&","\\&")).arg(suggested_id).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0);
    }

    this->sendMessage(command);
}

void ParrotZikTweeker::downloadPreset(QString name, QString album, int linkAlbum, QString artist, int linkArtist, QString genre, int linkGenre, QString song, int linkSong,  int spatializerEnabled, int room, int angle, QString equa, int id, int modelNumber)
{
    QString command;

    int finalModelNumber = modelNumber;

    if(finalModelNumber == -1){
        if(model() == "ZIK1"){
            finalModelNumber = 1;
        }else if(model() == "ZIK2"){
            finalModelNumber = 3;
        }else if(model() == "ZIK3"){
            finalModelNumber = 5;
        }else if(model() == "ZIKSPORT"){
            finalModelNumber = 4;
        }
    }

    //qDebug() << "HEY ! Is is enable ? " << (spatializerEnabled == 1 ? "HELL YEAH !" : "Nope.");

    if(id < 0){
        command = QString("GET /api/audio/preset/download?name=%1&album=%2&link_album=%3&artist=%4&link_artist=%5&genre=%6&link_genre=%7&song=%8&link_song=%9&%10equa=%12&modelNumber=%13").arg(name.replace("&","\\&")).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0).arg(spatializerEnabled ? QString("room=%1&angle=%2&").arg(room).arg(angle) : QString("")).arg(equa).arg(finalModelNumber);
        //QString command("GET /api/audio/preset/download?name=Download1&album=&link_album=0&artist=Haha&link_artist=1&genre=&link_genre=0&song=Baby&link_song=1&room=2&angle=60&equa=70,300,1000,4000,10000,1,1,1,1,1,-5,-5,-5,-5,-5");
    }else{
        command = QString("GET /api/audio/preset/download?name=%1&id=%2&album=%3&link_album=%4&artist=%5&link_artist=%6&genre=%7&link_genre=%8&song=%9&link_song=%10&%11equa=%12&modelNumber=%13").arg(name.replace("&","\\&")).arg(id).arg(linkAlbum ? album.replace("&","\\&") : "").arg(linkAlbum ? 1 : 0).arg(linkArtist ? artist.replace("&","\\&") : "").arg(linkArtist ? 1 : 0).arg(linkGenre ? genre.replace("&","\\&") : "").arg(linkGenre ? 1 : 0).arg(linkSong ? song.replace("&","\\&") : "").arg(linkSong ? 1 : 0).arg(spatializerEnabled ? QString("room=%1&angle=%2&").arg(room).arg(angle) : QString("")).arg(equa).arg(finalModelNumber);
    }

    this->sendMessage(command);
}

void ParrotZikTweeker::deletePreset(int id)
{
    QString command = QString("GET /api/audio/preset/remove?id=%1").arg(id);

    qDebug() << "Sending message : " << command;
    this->sendMessage(command);

    presetModel->deletePreset(id);
}

void ParrotZikTweeker::enablePreset(int id, bool status)
{
    QString command = QString("GET /api/audio/preset/activate?id=%1&enable=%2").arg(id).arg(status ? 1 : 0);

    this->sendMessage(command);

    presetModel->enablePreset(id, status);
}


void ParrotZikTweeker::setSmartAudioTune(const QString &enable)
{
    if (enable != m_smartAudioTune) {
        QString command("GET /api/audio/smart_audio_tune/set?arg=");

        command = command.append(enable);

        m_smartAudioTune = enable;
        emit smartAudioTuneChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setPresetBypass(const QString &enable)
{
    if (enable != m_presetBypass) {
        QString command("GET /api/audio/preset/bypass/set?arg=");

        command = command.append(enable);

        m_presetBypass = enable;
        emit presetBypassChanged();

        this->sendMessage(command);
    }
}

void ParrotZikTweeker::setPresetCounter(const QString &counter)
{
    if (counter != m_presetCounter) {

        m_presetCounter = counter;
        emit presetCounterChanged();
    }
}

void ParrotZikTweeker::cancelProducerPreset()
{
    QString command("GET /api/audio/preset/cancel_producer");

    this->sendMessage(command);
}

void ParrotZikTweeker::clearAllPresets()
{
    QString command("GET /api/audio/preset/clear_all");
    this->sendMessage(command);

}

void ParrotZikTweeker::startPresetSynchronization()
{
    QString command("GET /api/audio/preset/synchro/start");
    this->sendMessage(command);
}


void ParrotZikTweeker::stopPresetSynchronization()
{
    QString command("GET /api/audio/preset/synchro/stop");
    this->sendMessage(command);
}
