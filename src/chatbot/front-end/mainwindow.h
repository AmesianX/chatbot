/*
 * Copyright (C) 2012 Andres Pagliano, Gabriel Miretti, Gonzalo Buteler,
 * Nestor Bustamante, Pablo Perez de Angelis
 *
 * This file is part of LVK Chatbot.
 *
 * LVK Chatbot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LVK Chatbot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LVK Chatbot.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LVK_FE_MAINWINDOW_H
#define LVK_FE_MAINWINDOW_H

#include <QMainWindow>

#include "back-end/appfacade.h"
#include "back-end/rule.h"
#include "common/conversation.h"
#include "front-end/mainwindowrefactor.h"

class TestMainWindow;
class QModelIndex;
class QFile;
class QLabel;

namespace Ui
{
    class MainWindow;
}

class QItemSelectionModel;
class QItemSelection;

namespace Lvk
{

/// \addtogroup Lvk
/// @{

namespace BE
{
    class ConversationWriter;
}

namespace DAS
{
    class UpdateInfo;
}

namespace FE
{

/// \ingroup Lvk
/// \addtogroup FE
/// @{

class RuleTreeModel;
class TinyScoreWidget;

/**
 * \brief The MainWindow class provides the application main window.
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class ::TestMainWindow;

public:

    /**
     * Constructs a MainWindow with \a parent.
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * Destroys the window.
     */
    ~MainWindow();

    /**
     * Clears the content of the window including rules, test chats, chat history and disconnects
     * from chat
     */
    void clear();

    /**
     * Creates a new a chatbot with the given \filename and sets the window with
     * verification account mode.\a filename can be empty in which case the save dialog
     * will be opened.
     */
    void newFile(const QString &filename);

    /**
     * Opens the given file and sets the window with edit mode
     */
    void openFile(const QString &filename);

    /**
     * Opens the last opened file (if any) and sets the window with edit mode
     */
    void openLastFile();

protected:
    virtual bool event(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    MainWindow(MainWindow&);
    MainWindow& operator=(MainWindow&);

    Ui::MainWindow          *ui;
    BE::AppFacade           *m_appFacade;
    MainWindowRefactor       m_refactor;
    RuleTreeModel           *m_ruleTreeModel;
    QItemSelectionModel     *m_ruleTreeSelectionModel;
    bool                     m_ruleEdited;
    bool                     m_ruleAdded;
    QString                  m_filename;
    TinyScoreWidget         *m_tinyScore;

    void setupUi();

    void clear(bool resetModel);

    BE::Rule *addCategory(const QString &name);
    BE::Rule *addRule(const QString &name, BE::Rule *category);

    const Lvk::BE::Rule * findRule(quint64 ruleId) const;
    void showRule(quint64 ruleId);

    BE::Rule *addCategoryWithDialog();
    BE::Rule *addRuleWithDialog(const QStringList &tmplInput, const QStringList &tmplOutput);
    bool removeSelectedRuleWithDialog();

    void setUiMode(UiMode mode);

    bool initWithFile(const QString &filename, bool newFile = false);
    void connectSignals();

    void loadAllSettings();
    void loadMainWindowSettings();
    void loadSplittersSettings();
    void loadChatSettings();

    void saveAllSettings();
    void saveMainWindowSettings();
    void saveSplittersSettings();
    void saveChatSettings();

    void startEditMode();

    void selectFirstRule();
    void selectRule(const BE::Rule *rule);

    inline BE::Rule *rootRule();
    inline const BE::Rule *rootRule() const;
    inline BE::Rule *evasivesRule();
    inline BE::Rule *selectedRule();

    void teachRule(BE::Rule *rule);
    void undoRule(BE::Rule *rule);
    void handleRuleEdited(BE::Rule *rule);
    void ruleEditFinished();

    void highlightMatchedRules(const BE::AppFacade::MatchList &matches);
    void setCurrentCategory(quint64 catId);

    void undoRuleEdited(BE::Rule *rule);

    void showRuleOnWidget(const BE::Rule *rule);
    void refreshRuleOnWidget();

    QString getRuleDisplayName(const QModelIndex &index) const;
    QString getRuleDisplayName(const BE::Rule *rule) const;

    inline QString canonicAccount();

    bool hasUnsavedChanges();
    int showSaveChangesDialog();
    bool saveChanges();
    bool saveAsChanges();
    bool load(const QString &filename);
    void setFilename(const QString &filename);

    void setNlpEngineOption(BE::AppFacade::NlpEngineOption option, bool enabled);
    bool nlpEngineOption(BE::AppFacade::NlpEngineOption option);

    void setPccWidgetsVisible(bool visible);

    void uploadContestData();

    void updateScore();
    void uploadBlockedForUpdate(const DAS::UpdateInfo &info);
    void updateTinyScorePos();

private slots:
    void onAddCategoryButtonClicked();
    void onAddRuleButtonClicked();
    void onRemoveButtonClicked();
    void onAddVarRuleAction();
    void onAddCondRuleAction();

    BE::Rule *getCategoryFromDialog();
    void onTeachFromHistory(const QString &msg);
    void onHistoryShowRule(quint64 ruleId);
    void onRemovedAllHistory();
    void onRemovedHistory(const QDate &date, const QString &username);

    void onRuleInputEdited(const QString &input);
    void onRuleAdded();
    void onRuleEdited();
    void onRuleSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onTeachRule();
    void onUndoRule();
    void onRulesRemoved(const QModelIndex &parent, int first, int last);

    void onTestInputTextEntered();
    void onTestTargetChanged();
    void onClearTestConvPressed();
    void onTestShowRule();

    void onVerifyAccountOk();
    void onVerifyAccountError(int err, const QString &msg);
    void onVerifyAccountSkipped();
    void onVerifyAccountCanceled();

    void onConnectionOk();
    void onConnectionError(int err);
    void onDisconnection();
    void onRosterSelChanged();

    void onNewChatConversation(const Cmn::Conversation::Entry &entry);

    void onNewMenuTriggered();
    void onOpenMenuTriggered();
    void onOpenLastFileMenuTriggered();
    void onSaveMenuTriggered();
    void onSaveAsMenuTriggered();
    void onImportMenuTriggered();
    void onExportMenuTriggered();
    void onAboutMenuTriggered();
    void onOptionsMenuTriggered();
    void onExitMenuTriggered();

    void onSplitterMoved(int, int);
    void onCurrentTabChanged(QWidget *tab);

    void onUploadScore();
    void onScoreRemainingTime(int time);

    void onUpdate(const DAS::UpdateInfo &info);

    void onClueShowRule(quint64 ruleId);
};

/// @}

} // namespace Lvk

/// @}

} // namespace FE


#endif // LVK_FE_MAINWINDOW_H
