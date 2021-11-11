#ifndef PRESETMODEL_H
#define PRESETMODEL_H

#ifdef DEBUG
#include <QDebug>
#endif

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "parrotziktweaker.h"

class ParrotZikTweeker;

class PresetModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PresetModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash <int, QByteArray> roleNames() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    //QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    enum PresetRoles {
        IdRole = Qt::UserRole +1,
        NameRole,
        EnabledRole
    };



    int insertPreset(QString name,
                      bool spatializerEnabled, int roomSize, int angle,
                      QString album, bool linkAlbum,
                      QString artist, bool linkArtist,
                      QString genre, bool linkGenre,
                      QString song, bool linkSong, bool noWaitForId, int modelNumber);

    bool deletePreset(int id);
    void setEquaParameters(int freq1, int freq2, int freq3, int freq4, int freq5,
                           float q1, float q2, float q3, float q4, float q5,
                           float gain1, float gain2, float gain3, float gain4, float gain5);


    void saveToFile();
    void initPresets(QString remoteUsername, int remoteCounter);
    void setCounter(int c);

    void enablePreset(int id, bool status);
    int setCurrentPreset(int id);
signals:

public slots:
    void confirmPresetInsertion(int id);

private:
    QJsonArray presets;
    QJsonObject waitingForId;
    ParrotZikTweeker * tweaker;

    QString username;
    int counter;
    bool syncInProgress;

    int freq1; int freq2; int freq3; int freq4; int freq5;
    float q1; float q2; float q3; float q4; float q5;
    float gain1; float gain2; float gain3; float gain4; float gain5;
};

#endif // PRESETMODEL_H
