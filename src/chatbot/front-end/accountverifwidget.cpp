#include "front-end/accountverifwidget.h"
#include "front-end/updateexecutor.h"
#include "front-end/memberfunctor.h"
#include "common/globalstrings.h"
#include "ui_accountverifwidget.h"

#include <QMessageBox>
#include <QtDebug>
#include <cassert>

//--------------------------------------------------------------------------------------------------
// AccountVerifWidget
//--------------------------------------------------------------------------------------------------

Lvk::FE::AccountVerifWidget::AccountVerifWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::AccountVerifWidget), m_appFacade(0),
      m_uiMode(VerifyAccountUiMode)
{
    ui->setupUi(this);

#ifdef DA_CONTEST
    loadCharacters();
#else
    ui->charsComboBox->setVisible(false);
    ui->characterLabel->setVisible(false);
#endif // DA_CONTEST


    clear();

    connectSignals();
}

//--------------------------------------------------------------------------------------------------

Lvk::FE::AccountVerifWidget::~AccountVerifWidget()
{
    delete ui;
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::loadCharacters()
{
#ifdef DA_CONTEST
    ui->charsComboBox->clear();
    ui->charsComboBox->addItem(tr("(Choose one)"));

    if (m_appFacade) {
        foreach (const Clue::Character &c, m_appFacade->characters()) {
            ui->charsComboBox->addItem(c.name);
        }
    }
#endif // DA_CONTEST

}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::connectSignals()
{
    connect(ui->verifyAccountButton,   SIGNAL(clicked()),       SLOT(onVerifyPressed()));
    connect(ui->verifyLaterButton,     SIGNAL(clicked()),       SLOT(onVerifyAccountSkipped()));
    connect(ui->cancelChAccountButton, SIGNAL(clicked()),       SLOT(onCancelChAccountPressed()));
    connect(ui->passwordText,          SIGNAL(returnPressed()), SLOT(onVerifyPressed()));
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::setAppFacade(BE::AppFacade *appFacade)
{
    m_appFacade = appFacade;

    loadCharacters();
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::connectAppFacadeSignals(bool connected)
{
    if (m_appFacade) {
        // Connect these signals only if we are currently verifying an account.
        // If we have several instances of the widget, the disconnection avoids that
        // all instances receives the accountOk or accountError signal
        if (connected) {
            connect(m_appFacade, SIGNAL(accountOk()), SLOT(onAccountOk()));
            connect(m_appFacade, SIGNAL(accountError(int, QString)),
                    SLOT(onAccountError(int, QString)));
        } else {
            m_appFacade->disconnect(this);
        }
    } else {
        qCritical() << "AccountVerifWidget: No AppFacade set";
    }
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::onVerifyPressed()
{
    if (!verifyCharacter()) {
        return;
    }

    QString title;
    QString errMsg;

    QString username = ui->usernameText->text();

    if (username.isEmpty()) {
        title = tr("Invalid username");
        errMsg = tr("Please, provide a username");
    }

    // To connect to a facebook chat we need the facebook username. We cannot connect using the
    // user's email. The 'Dale Aceptar' verification mechanism provide us of the facebook
    // username so this check only makes sense for non-'Dale Aceptar' versions because
    #ifndef DA_CONTEST
    if (ui->fbChatRadio->isChecked() && username.contains("@") &&
               !username.contains("@facebook.com")) {
        title = tr("Invalid username");
        errMsg = tr("To connect you need to provide your Facebook username instead of your "
                    "email.<br/><br/>You don't have or remember your username? "
                    "<a href=\"http://www.facebook.com/username\">Click here</a>");
    }
    #endif

    if (errMsg.isEmpty()) {
        setUiMode(VerifyingAccountUiMode);

    #ifdef DA_CONTEST
        FE::MemberFunctor<AccountVerifWidget> *f = new FE::MemberFunctor<AccountVerifWidget>(this,
                            &AccountVerifWidget::verifyBlockedForUpdate,
                            &AccountVerifWidget::verifyAccount);

        UpdateExecutor::exec(f, isCritical);
    #else
        verifyAccount();
    #endif
    } else {
        QMessageBox::information(this, title, errMsg);

        ui->usernameText->setFocus();
    }
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::verifyAccount()
{
    assert(m_appFacade);

    qDebug() << "AccountVerifWidget: Verifying Account...";

    connectAppFacadeSignals(true);

    QString username = ui->usernameText->text();
    QString password = ui->passwordText->text();
    BE::ChatType type = ui->gtalkChatRadio->isChecked() ? BE::GTalkChat : BE::FbChat;

    m_appFacade->verifyAccount(type, username, password);
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::verifyBlockedForUpdate(const DAS::UpdateInfo &info)
{
    qDebug() << "AccountVerifWidget: Verify blocked for update";

    connectAppFacadeSignals(false);

    setUiMode(VerifyAccountUiMode);

    emit blockedForUpdate(info);
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::onAccountOk()
{
    qDebug() << "AccountVerifWidget: Verify Account Ok";

    connectAppFacadeSignals(false);

    setCurrentCharacter();

    clear();

    emit accountOk();
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::onAccountError(int err, const QString &msg)
{
    qDebug() << "AccountVerifWidget: Verify Account Error" << err << msg;

    connectAppFacadeSignals(false);

    setUiMode(VerifyAccountFailedUiMode);

    emit accountError(err, msg);
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::onVerifyAccountSkipped()
{
    if (!verifyCharacter()) {
        return;
    }

    qDebug() << "AccountVerifWidget: Verify Account Skipped";

    connectAppFacadeSignals(false);

    setCurrentCharacter();

    clear();

    emit verificationSkipped();
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::onCancelChAccountPressed()
{
    assert(m_appFacade);

    qDebug() << "AccountVerifWidget: Verify Account Canceled";

    connectAppFacadeSignals(false);

    m_appFacade->cancelVerifyAccount();

    setUiMode(VerifyAccountUiMode);

    if (m_uiMode != VerifyingAccountUiMode) {
        emit verificationCanceled();
    }
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::setUiMode(Lvk::FE::AccountVerifWidget::UiMode mode)
{
    switch (mode) {

    case VerifyAccountUiMode:
        ui->verifyAccountButton->setEnabled(true);
        ui->charsComboBox->setEnabled(true);
        ui->usernameText->setEnabled(true);
        ui->passwordText->setEnabled(true);
        ui->fbChatRadio->setEnabled(true);
        ui->gtalkChatRadio->setEnabled(gtalkEnabled());
        ui->verifyLaterButton->setEnabled(true);
        ui->connectionProgressBar->setVisible(false);
        ui->connectionStatusLabel->setVisible(false);
        break;

    case VerifyingAccountUiMode:
        ui->verifyAccountButton->setEnabled(false);
        ui->charsComboBox->setEnabled(false);
        ui->usernameText->setEnabled(false);
        ui->passwordText->setEnabled(false);
        ui->fbChatRadio->setEnabled(false);
        ui->gtalkChatRadio->setEnabled(false);
        ui->verifyLaterButton->setEnabled(false);
        ui->connectionProgressBar->setVisible(true);
        ui->connectionStatusLabel->setVisible(true);
        break;

    case VerifyAccountFailedUiMode:
        ui->verifyAccountButton->setEnabled(true);
        ui->charsComboBox->setEnabled(true);
        ui->usernameText->setEnabled(true);
        ui->passwordText->setEnabled(true);
        ui->fbChatRadio->setEnabled(true);
        ui->gtalkChatRadio->setEnabled(gtalkEnabled());
        ui->verifyLaterButton->setEnabled(true);
        ui->connectionProgressBar->setVisible(false);
        ui->connectionStatusLabel->setVisible(false);
        break;
    }

    m_uiMode = mode;
}

//--------------------------------------------------------------------------------------------------

inline bool Lvk::FE::AccountVerifWidget::gtalkEnabled() const
{
#ifdef DA_CONTEST
    return false;
#else
    // Gtalk does not use XMPP anymore and it is not supported
    //return true;
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::clear()
{
    ui->fbChatRadio->setChecked(true);
    ui->passwordText->clear();
    ui->usernameText->clear();
    ui->charsComboBox->setCurrentIndex(0);

    setUiMode(VerifyAccountUiMode);
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::setChangeAccountMode(bool changeMode)
{
    ui->verifyLaterButton->setVisible(!changeMode);
#ifdef DA_CONTEST
    ui->charsComboBox->setVisible(!changeMode);
    ui->characterLabel->setVisible(!changeMode);
#endif
}

//--------------------------------------------------------------------------------------------------

bool Lvk::FE::AccountVerifWidget::verifyCharacter()
{
    if (ui->charsComboBox->isVisible() && ui->charsComboBox->currentIndex() == 0) {
        QMessageBox::information(this, tr("Invalid character"), tr("Please, choose a character."));
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------

void Lvk::FE::AccountVerifWidget::setCurrentCharacter()
{
#ifdef DA_CONTEST
    if (ui->charsComboBox->isVisible()) {
        m_appFacade->setCurrentCharacter(ui->charsComboBox->currentText());
        m_appFacade->save();
    }
#endif // DA_CONTEST
}

