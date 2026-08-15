#ifndef CAPTURECHOICEDIALOG_H
#define CAPTURECHOICEDIALOG_H

#include <optional>
#include <vector>

#include <QDialog>

#include "move.h"

class CaptureChoiceDialog : public QDialog {
    Q_OBJECT

public:
    // Mostra o dialogo modal e retorna a opcao escolhida pelo jogador. So deve ser
    // chamado quando 'options' tiver mais de um elemento (ambiguidade real).
    static MoveOption choose(QWidget *parent, const std::vector<MoveOption> &options);

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    explicit CaptureChoiceDialog(const std::vector<MoveOption> &options, QWidget *parent);

    std::optional<MoveOption> m_chosen;
};

#endif // CAPTURECHOICEDIALOG_H
