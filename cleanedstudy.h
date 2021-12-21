#ifndef CLEANEDSTUDY_H
#define CLEANEDSTUDY_H

#include <QWidget>
#include <QSqlDatabase>
#include <QJsonObject>
#include <seasonmodel.h>
#include <livestockmodel.h>
#include <feedsmodel.h>
#include <cropinputsmodel.h>
#include <fertilizermodel.h>
#include "basketmodel.h"
#include <QProcess>

namespace Ui {
class CleanedStudy;
}

class CleanedStudy : public QWidget
{
    Q_OBJECT

public:
    explicit CleanedStudy(QWidget *parent = nullptr);
    ~CleanedStudy();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool run();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void setDatabase(QString databaseFile);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void documentWasModified();
    void modelFinished(int exitCode);
    void on_cmd_add_season_clicked();

    void on_cmd_del_season_clicked();

    void on_cbm_soiltype_currentIndexChanged(int index);

    void on_cbm_cropland_system_currentIndexChanged(int index);

    void on_cbm_cropland_tillage_currentIndexChanged(int index);

    void on_cbm_cropland_orgmatter_currentIndexChanged(int index);

    void on_cbm_grassland_management_currentIndexChanged(int index);

    void on_cbm_grassland_implevel_currentIndexChanged(int index);

    void on_cmd_add_live_clicked();

    void on_cmd_add_feed_clicked();

    void on_cmd_add_fert_clicked();

    void season_changed(QString fromSeason, QString toSeason);

    void on_cmd_remove_live_clicked();

    void on_cmd_remove_feed_clicked();

    void on_cmd_remove_fert_clicked();

    void modelChanged();
    void textEdited(QString text);

    void on_cbm_climate_currentIndexChanged(const QString &arg1);

    void on_livestockView_doubleClicked(const QModelIndex &index);

    void on_feedsView_doubleClicked(const QModelIndex &index);

    void on_tabWidget_currentChanged(int index);

signals:
    void report_error(QString error_message);

private:
    Ui::CleanedStudy *ui;
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QSqlDatabase db;
    QString curFile;
    QString curFilePath;
    bool isUntitled;
    bool model_running;
    void load_models();
    void saveStudyObject();
    void loadStudyObject();
    QVector <int> livestock_colums;
    QVector <int> feed_colums;
    QJsonObject study_object;    
    bool studyModified;
    //Models
    seasonModel *m_seasons;
    livestockModel *m_livestock;
    feedsModel *m_feeds;
    cropInputsModel *m_cropInputs;
    fertilizerModel *m_fertilizer;
    basketModel * m_basktet;
    bool loading;
    void BrowseChildren(QWidget * parent);
    QProcess *m_process;
};

#endif // CLEANEDSTUDY_H
