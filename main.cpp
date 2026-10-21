#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QTimer>
#include <QEventLoop>
#include <vector>
#include <iostream>
#include <cmath>
#include <utility>

class NodeItem : public QGraphicsEllipseItem {
public:
    NodeItem(qreal x, qreal y, qreal radio) 
        : QGraphicsEllipseItem(-radio, -radio, radio*2, radio*2) {
        setPos(x, y); 
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges); 
        setZValue(1); 
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionHasChanged) {
            updLines();
            for (const auto& connection : connections) {
                connection.node->updLines();
            }
        }
        return QGraphicsEllipseItem::itemChange(change, value);
    }

    void connectWith(NodeItem* otherNode, QGraphicsScene* escene) {
        auto* line = new QGraphicsLineItem();
        line->setPen(QPen(Qt::black, 3));
        line->setZValue(0); 
        escene->addItem(line);
        
        connections.append({otherNode, line});
        otherNode->connections.append({this, line});
        
        updLines();
        otherNode->updLines();
    }

    void updLines() {
        for (const auto& connection : connections) {
            QPointF cThis = scenePos();
            QPointF cElse = connection.node->scenePos();
            connection.line->setLine(QLineF(cThis, cElse));
        }
    }

private:
    struct Connection {
        NodeItem* node;
        QGraphicsLineItem* line;
    };
    QList<Connection> connections;
};

void addText(QString text, NodeItem* node) {
    auto* itemText = new QGraphicsSimpleTextItem(text, node);
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    itemText->setFont(font);
    
    QRectF rect = itemText->boundingRect();
    itemText->setPos(-rect.width()/2, -rect.height()/2);
    itemText->setZValue(2); 
}

class mainWindow : public QMainWindow {
public:
    mainWindow(const std::vector<NodeItem*>& nodesRef, const std::vector<std::vector<int>>& adjRef) : nodes(nodesRef), adj(adjRef) {
        
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);

        QPushButton* btnRun = new QPushButton("RUN ALGORITHM", this);
        btnRun->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px; background-color: #4CAF50; color: white;");
        
        connect(btnRun, &QPushButton::clicked, this, &mainWindow::startAlgorithm);

        layout->addWidget(btnRun);
        
        setCentralWidget(centralWidget);
        resize(650, 700);
    }

    void startAlgorithm() {
        QPushButton* btn = findChild<QPushButton*>();
        if(btn) btn->setEnabled(false);

        algorithmVisualize(nodes, adj, 1);
        
        if(btn) btn->setEnabled(true);
    }

private:
    const std::vector<NodeItem*>& nodes;
    const std::vector<std::vector<int>>& adj;

    void algorithmVisualize(const std::vector<NodeItem*>& nodes, const std::vector<std::vector<int>>& adj, int node, int previousNode = 0) {

        nodes[node]->setBrush(QBrush(Qt::red));
        if (nodes[node]->scene()) nodes[node]->scene()->update();

        QEventLoop loop;
        QTimer::singleShot(1000, &loop, &QEventLoop::quit);
        loop.exec(); 

        for (auto u : adj[node]) {
            if (u == previousNode) continue;
            algorithmVisualize(nodes, adj, u, node);
        }

        nodes[node]->setBrush(QBrush(Qt::gray));
        if (nodes[node]->scene()) nodes[node]->scene()->update();
    }
};


int main(int argc, char *argv[]) {
    int N, M;
    if (!(std::cin >> N >> M)) {
        std::cerr << "Error: Couldn't read input.\n";
        return 1;
    }

    std::vector<std::array<int, 2>> edges(M);
    for (int i = 0; i < M; ++i)
        std::cin >> edges[i][0] >> edges[i][1];

    QApplication app(argc, argv);
    
    QGraphicsScene* scene = new QGraphicsScene(); 
    scene->setSceneRect(0, 0, 600, 600);

    std::vector<NodeItem*> nodes(N + 1, nullptr);
    qreal radio = 25.0;
    qreal cX = 300, cY = 300;
    qreal rCircle = 200.0; 

    for (int i = 1; i <= N; ++i) {
        qreal angle = 2 * M_PI * (i - 1) / N; 
        qreal x = cX + rCircle * std::cos(angle);
        qreal y = cY + rCircle * std::sin(angle);

        auto* node = new NodeItem(x, y, radio);
        node->setBrush(QBrush(Qt::gray));
        scene->addItem(node);
        
        nodes[i] = node;
        addText(QString::number(i), node);
    }

    for (int i = 0; i < M; ++i) {
        int u = edges[i][0];
        int v = edges[i][1];

        if (1 <= u && u <= N && 1 <= v && v <= N) {
            nodes[u]->connectWith(nodes[v], scene); 
        } else {
            std::cerr << "Error: Invalid edge between " << u << " and " << v << "\n";
        }
    }

    std::vector<std::vector<int>> adj(N + 1);
    for (int i = 0; i < M; ++i) {
        adj[edges[i][0]].push_back(edges[i][1]);
        adj[edges[i][1]].push_back(edges[i][0]);
    }

    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing); 
    
    mainWindow window(nodes, adj);
    
    QWidget* central = window.centralWidget();
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(central->layout());
    if (layout) {
        layout->addWidget(view);
    }

    window.show();

    return app.exec();
}