#include "registerwindow.h"
#include "ui_registerwindow.h"
#include "binapi.h"
#include "pcloudapp.h"

RegisterWindow::RegisterWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegisterWindow)
{
    app=a;
    setWindowIcon(QIcon(":/images/images/icon_pcloud.png"));
    ui->setupUi(this);
    connect(ui->registerButton, SIGNAL(clicked()), this, SLOT(doRegister()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(focusConfirm()));
    connect(ui->confirmpassword, SIGNAL(returnPressed()), this, SLOT(focusTOS()));
    connect(ui->accepttos, SIGNAL(returnPressed()), this, SLOT(doRegister()));
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::setError(const char *err){
    ui->error->setText(err);
}

void RegisterWindow::focusPass(){
    ui->password->setFocus();
}

void RegisterWindow::focusConfirm(){
    ui->confirmpassword->setFocus();
}

void RegisterWindow::focusTOS(){
    ui->accepttos->setFocus();
}

void RegisterWindow::doRegister(){
    if (!ui->accepttos->checkState()){
        setError("Registration only possible upon acceptanse of terms.");
        return;
    }
    if (ui->password->text().length()<6){
        setError("Password too short - minimum 6 characters.");
        return;
    }
    if (ui->password->text()!=ui->confirmpassword->text()){
        setError("Password confirmation does not match.");
        return;
    }
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    apisock *conn;
    binresult *res, *result;
    QByteArray err;
    if (!(conn=app->getAPISock())){
        setError("Connection to server failed.");
        return;
    }
    res=send_command(conn, "register",
                     P_STR("termsaccepted", "yes"),
                     P_LSTR("mail", email.constData(), email.size()),
                     P_LSTR("password", password.constData(), password.size()));
    result=find_res(res, "result");
    if (!result){
        setError("Connection to server failed.");
        free(res);
        api_close(conn);
        return;
    }
    if (result->num!=0){
        setError(find_res(res, "error")->str);
        free(res);
        api_close(conn);
        return;
    }
    res=send_command(conn, "userinfo",
                     P_LSTR("username", email.constData(), email.size()),
                     P_LSTR("password", password.constData(), password.size()),
                     P_BOOL("getauth", 1));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        setError("Connection to server failed.");
        free(res);
        return;
    }
    if (result->num!=0){
        setError(find_res(res, "error")->str);
        free(res);
        return;
    }
    if (!app->userLogged(res, err))
        setError(err);
    else{
        setError("");
        ui->password->clear();
        ui->confirmpassword->clear();
        hide();
    }
    free(res);
}
