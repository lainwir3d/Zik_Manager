#include "presetmodel.h"

PresetModel::PresetModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    tweaker = (ParrotZikTweeker *) parent;
    syncInProgress = false;
}

QHash <int, QByteArray> PresetModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[EnabledRole] = "p_enabled";
    return roles;
}

int PresetModel::columnCount(const QModelIndex &parent) const
{
    return 28;
}

int PresetModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0;
    }else{
        return presets.size();
    }
}

QVariant PresetModel::data(const QModelIndex &index, int role) const
{
    QJsonObject p = presets[index.row()].toObject();
    //qDebug() << QJsonDocument(p).toJson();
    QString columnName = "null";

    switch(role){
        case IdRole:
            columnName = "id";
            break;
        case NameRole:
            columnName = "name";
            break;
        case EnabledRole:
            columnName = "enabled";
            break;
    }

    if(columnName.compare("null") != 0){
        QJsonValue v = p.value(columnName);

        if(v.isString()) return v.toString();
        else if(v.isDouble()) return v.toVariant().toInt();
        else if(v.isBool()) return v.toBool();
        else return QVariant();

    }else{
        //qDebug() << "returning QVariant";
        return QVariant();
    }
}

void PresetModel::setEquaParameters(int freq1, int freq2, int freq3, int freq4, int freq5,
                       float q1, float q2, float q3, float q4, float q5,
                       float gain1, float gain2, float gain3, float gain4, float gain5)
{
    this->freq1 = freq1;
    this->freq2 = freq2;
    this->freq3 = freq3;
    this->freq4 = freq4;
    this->freq5 = freq5;

    this->q1 = q1;
    this->q2 = q2;
    this->q3 = q3;
    this->q4 = q4;
    this->q5 = q5;

    this->gain1 = gain1;
    this->gain2 = gain2;
    this->gain3 = gain3;
    this->gain4 = gain4;
    this->gain5 = gain5;
}

int PresetModel::insertPreset(QString name, bool spatializerEnabled, int roomSize, int angle, QString album, bool linkAlbum, QString artist, bool linkArtist, QString genre, bool linkGenre, QString song, bool linkSong, bool noWaitForId, int modelNumber)
{
    QJsonObject o;

    o["name"] = name;
    o["enabled"] = false;
    o["fc0"] = freq1;
    o["fc1"] = freq2;
    o["fc2"] = freq3;
    o["fc3"] = freq4;
    o["fc4"] = freq5;
    o["q0"] = q1;
    o["q1"] = q2;
    o["q2"] = q3;
    o["q3"] = q4;
    o["q4"] = q5;
    o["gain0"] = gain1;
    o["gain1"] = gain2;
    o["gain2"] = gain3;
    o["gain3"] = gain4;
    o["gain4"] = gain5;
    o["spatializerEnabled"] = spatializerEnabled ? 1 : 0;
    o["spatializerRoom"] = roomSize;
    o["spatializerAngle"] = angle;
    o["album"] = album;
    o["albumLinked"] = linkAlbum ? 1 : 0;
    o["artist"] = artist;
    o["artistLinked"] = linkArtist ? 1 : 0;
    o["genre"] = genre;
    o["genreLinked"] = linkGenre ? 1 : 0;
    o["song"] = song;
    o["songLinked"] = linkSong ? 1 : 0;
    o["modelNumber"] = modelNumber;

    if(noWaitForId){
        int rowc = rowCount(QModelIndex());
        this->beginInsertRows(QModelIndex(), rowc, rowc);
        presets.append(o);
        this->endInsertRows();

        this->saveToFile();
    }else{
        waitingForId = o;
    }

    //qDebug() << QJsonDocument(presets).toJson();

    return this->rowCount(QModelIndex())+1;
}

void PresetModel::confirmPresetInsertion(int id)
{
    if(!syncInProgress){
        if(!waitingForId.empty()){
            waitingForId["id"] = id;

            int rowc = rowCount(QModelIndex());
            this->beginInsertRows(QModelIndex(), rowc, rowc);
            presets.append(waitingForId);
            this->endInsertRows();

            waitingForId = QJsonObject();

            saveToFile();

            //qDebug() << "Preset " << id << "saved";
        }
    }else{
        qDebug("confirmPresetInsertion: Sync in progress ! Not inserting");
    }
}

bool PresetModel::deletePreset(int id)
{
    for(int i=0; i<presets.size();i++){
        QJsonObject o = presets[i].toObject();

        if(o.value("id").toVariant().toInt() == id){
            this->beginRemoveRows(QModelIndex(), i, i);
            presets.removeAt(i);
            this->endRemoveRows();
        }
    }

    saveToFile();

    return true;
}

void PresetModel::saveToFile()
{
    QJsonDocument doc;
    QJsonObject docobj;

    docobj["username"] = username;
    docobj["counter"] = counter;
    docobj["presets"] = presets;

    //qDebug() << "Saving presets to file. name: " << docobj.value("username").toString() << " / counter: " << docobj.value("counter").toVariant().toInt();

    QString datalocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir datadir(datalocation);
    //qDebug() << datadir.absolutePath();

    if(!datadir.exists()) {
        //qDebug() << "Data directory doesn't exist, creating it.";
        datadir.mkpath(datadir.absolutePath());
    }

    QFile f(datadir.absolutePath() + "/presets.json");
    //qDebug() << "File is : " + f.fileName();

    f.open(QIODevice::WriteOnly);
    //qDebug() << "File opened";

    f.write(QJsonDocument(docobj).toJson());
    //qDebug() << "File written";

    f.close();
}

void PresetModel::initPresets(QString remoteUsername, int remoteCounter)
{
    QJsonDocument doc;
    QString datalocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir datadir(datalocation);

    if(!datadir.exists()) {
        //qDebug() << "Data directory doesn't exist, creating it.";
        datadir.mkpath(datadir.absolutePath());
    }

    QFile f(datadir.absolutePath() + "/presets.json");

    if(f.exists()){
        qDebug() << "Opening presets.json";

        f.open(QIODevice::ReadOnly);
        doc = doc.fromJson(f.readAll());
        //qDebug() << doc.toJson();
        f.close();
        username = doc.object()["username"].toString();
        counter = doc.object().value("counter").toVariant().toInt();

        this->beginResetModel();
        presets = doc.object()["presets"].toArray();
        this->endResetModel();

        qDebug() << "Loaded " << presets.size() << " presets. Username is " << username << ". Counter is " << counter;
    }else{
        username = "QZik";
        counter = 0;
    }


    if((remoteUsername != username) || (remoteCounter != counter)) {
        qDebug() << "Headset presets not in sync with application. Syncing.";

        tweaker->startPresetSynchronization();
        syncInProgress = true;

        tweaker->clearAllPresets();
        qDebug() <<  "Preset Cleared !";

        for(int i=0; i<presets.size(); i++){
            QJsonObject o = presets.at(i).toObject();

            QString equa = QString::number(o.value("fc0").toVariant().toInt()) + "," + QString::number(o.value("fc1").toVariant().toInt()) + "," + QString::number(o.value("fc2").toVariant().toInt()) + "," + QString::number(o.value("fc3").toVariant().toInt()) + "," + QString::number(o.value("fc4").toVariant().toInt());
            equa += "," + QString::number(o.value("q0").toDouble()) + "," + QString::number(o.value("q1").toDouble()) + "," + QString::number(o.value("q2").toDouble()) + "," + QString::number(o.value("q3").toDouble()) + "," + QString::number(o.value("q4").toDouble());
            equa += "," + QString::number(o.value("gain0").toDouble()) + "," + QString::number(o.value("gain1").toDouble()) + "," + QString::number(o.value("gain2").toDouble()) + "," + QString::number(o.value("gain3").toDouble()) + "," + QString::number(o.value("gain4").toDouble());

            tweaker->downloadPreset(
                    o.value("name").toString(), o.value("album").toString(), o.value("albumLinked").toVariant().toInt(), o.value("artist").toString(), o.value("artistLinked").toVariant().toInt(),
                    o.value("genre").toString(), o.value("genreLinked").toVariant().toInt(), o.value("song").toString(), o.value("songLinked").toVariant().toInt(),
                    o.value("spatializerEnabled").toVariant().toInt(), o.value("spatializerRoom").toVariant().toInt(), o.value("spatializerAngle").toVariant().toInt(),
                    equa,
                    o.value("id").toVariant().toInt(),
                    o.value("modelNumber").toVariant().toInt()
            );
        }
        tweaker->stopPresetSynchronization();

        tweaker->setAccountUsername(username);

        if(presets.size() == 0) {
            counter = 0;
            saveToFile();
        }
        syncInProgress = false;
    }

}

void PresetModel::setCounter(int c)
{
    counter = c;
}

bool PresetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QString columnName = "null";

    switch(role){
        case IdRole:
            columnName = "id";
            break;
        case NameRole:
            columnName = "name";
            break;
        case EnabledRole:
            columnName = "enabled";
            break;
    }

    //qDebug() << "Setting role " << role << " to " << value.toBool();

    QJsonObject o = presets[index.row()].toObject();
    o[columnName] = value.toBool();
    presets[index.row()] = o;

//qDebug() << "Role " << role << " is " << presets[index.row()].toObject()[columnName].toBool();


    QVector<int> r = QVector<int>();
    r.append(EnabledRole);

    emit dataChanged(index, index, r);

    return true;
}

void PresetModel::enablePreset(int id, bool status)
{
    for(int i=0; i<presets.size();i++){
        QJsonObject o = presets[i].toObject();

        QModelIndex idx = this->createIndex(i,0);

        if(o.value("id").toVariant().toInt() == id){
            this->setData(idx, status, EnabledRole);
        }else{
            if(status == true){
                this->setData(idx, false, EnabledRole);
            }
        }
    }

    saveToFile();
}

int PresetModel::setCurrentPreset(int id)
{
    for(int i=0; i<presets.size();i++){
        QJsonObject o = presets[i].toObject();

        QModelIndex idx = this->createIndex(i,0);

        if(o.value("id").toVariant().toInt() == id){
            this->setData(idx, true, EnabledRole);
        }else{
            if(o.value("enabled").toVariant().toBool() == true){
                this->setData(idx, false, EnabledRole);
            }
        }
    }

    saveToFile();

    return id;
}
