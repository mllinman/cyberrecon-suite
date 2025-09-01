#pragma once
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class AttackGraph : public QWidget {
    Q_OBJECT
public:
    explicit AttackGraph(QWidget *parent = nullptr);

private slots:
    void generateGraph();
    void clearGraph();
    void exportGraph();
    void updateGraph();

private:
    void createNode(int x, int y, const QString &title, const QString &color, const QString &description);
    void createConnection(int x1, int y1, int x2, int y2);
    
    QGraphicsView *view;
    QGraphicsScene *scene;
};