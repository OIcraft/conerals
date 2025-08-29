namespace DeepSeek {

    // ȫ��״̬ (1-indexed)
    int selfId = -1;
    int generalX = -1;
    int generalY = -1;
    int turnCount = 0;

    // ��������״̬
    bool earlyGame = true;
    std::vector<std::pair<int, int>> expansionFrontiers;
    int lastFrontierUpdate = -10;

    // �߼���ͼ״̬ (1-indexed)
    std::vector<std::vector<char>> gridType;
    std::vector<std::vector<int>> gridArmy;
    std::vector<std::vector<int>> gridBelong;

    // ��������
    const int dx[4] = {0, -1, 0, 1}; // �ҡ��ϡ�����
    const int dy[4] = {1, 0, -1, 0};

    // �ƶ���ʷ����ֹ�񵴣�
    std::set<std::tuple<int, int, int, int>> recentMoves; // (fromX, fromY, toX, toY)

    // �����������
    std::mt19937 rng;

    // ��ʼ������
    void Init() {
        selfId = -1;
        generalX = -1;
        generalY = -1;
        turnCount = 0;
        earlyGame = true;
        expansionFrontiers.clear();
        recentMoves.clear();
        rng.seed(time(0));

        // 1-indexed�����ʼ�� (+1��С)
        gridType.resize(mapRows + 1, std::vector<char>(mapCols + 1, '?'));
        gridArmy.resize(mapRows + 1, std::vector<int>(mapCols + 1, 0));
        gridBelong.resize(mapRows + 1, std::vector<int>(mapCols + 1, -1)); // Ĭ�ϲ��ɼ�
    }

    // ������������
    void updateExpansionFrontiers() {
        // ÿ5�غϸ���һ�α߽�
        if (turnCount - lastFrontierUpdate < 5) return;
        lastFrontierUpdate = turnCount;
        expansionFrontiers.clear();

        // 1-indexedѭ��
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // �������ɼ��͵з�����
                if (gridType[i][j] == '?' || gridBelong[i][j] != selfId) continue;

                bool hasExpandable = false;

                // ����������ڸ���
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d];
                    int nj = j + dy[d];

                    // �߽��� (1-indexed)
                    if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                    // ��Ч����Ŀ�꣺����(0)��δ֪(?)
                    if (gridBelong[ni][nj] == 0 || gridType[ni][nj] == '?') {
                        hasExpandable = true;
                        break;
                    }
                }

                // ��ӵ����ű߽�
                if (hasExpandable) {
                    expansionFrontiers.push_back({i, j});
                }
            }
        }

        // ���û�б߽磬ʹ�ý���λ��
        if (expansionFrontiers.empty() && generalX != -1) {
            expansionFrontiers.push_back({generalX, generalY});
        }
    }

    // ������·���滮
    movement findFastestPathToBorder(int startX, int startY) {
        // BFSѰ������߽�
        std::queue<std::pair<int, int>> q;
        std::vector<std::vector<bool>> visited(mapRows + 1, std::vector<bool>(mapCols + 1, false));
        std::vector<std::vector<std::pair<int, int>>> parent(mapRows + 1,
            std::vector<std::pair<int, int>>(mapCols + 1, {-1, -1}));

        q.push({startX, startY});
        visited[startX][startY] = true;
        parent[startX][startY] = make_pair(startX, startY);

        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();

            // ����Ƿ�߽�
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                // 1-indexed�߽���
                if (nx < 1 || nx > mapRows || ny < 1 || ny > mapCols) continue;

                // ���ֿ���������������δ֪
                if (gridBelong[nx][ny] == 0 || gridType[nx][ny] == '?') {
                    // ���ݵ���һ��
                    int firstX = startX, firstY = startY;
                    int cx = x, cy = y;

                    // ����·���ҵ���һ��
                    while (parent[cx][cy].first != startX || parent[cx][cy].second != startY) {
                        auto [px, py] = parent[cx][cy];
                        cx = px;
                        cy = py;
                        firstX = cx;
                        firstY = cy;
                    }

                    // ȷ������
                    for (int dir = 0; dir < 4; dir++) {
                        if (firstX == startX + dx[dir] && firstY == startY + dy[dir]) {
                            movement m;
                            m.x = startX;
                            m.y = startY;
                            m.dir = dir;
                            return m;
                        }
                    }
                }
            }

            // ��չ�ھ�
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                // 1-indexed�߽���
                if (nx < 1 || nx > mapRows || ny < 1 || ny > mapCols) continue;
                if (visited[nx][ny]) continue;

                // ֻ��ͨ��������ȫ����
                if (gridBelong[nx][ny] == selfId || gridBelong[nx][ny] == 0) {
                    visited[nx][ny] = true;
                    parent[nx][ny] = {x, y};
                    q.push({nx, ny});
                }
            }
        }

        movement invalid;
        invalid.dir = 4;
        return invalid;
    }

    // �����ȫ���� (1-indexed)
    movement randomSafeExpand() {
        std::vector<std::tuple<int, int, int>> possibleMoves;

        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // ֻ����ɼ���������
                if (gridType[i][j] != '?' && gridBelong[i][j] == selfId && gridArmy[i][j] > 1) {
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dx[d];
                        int nj = j + dy[d];

                        // 1-indexed�߽���
                        if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                        // ������Ч����
                        if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                            gridType[ni][nj] == 'B' || gridType[ni][nj] == '?') continue;

                        // ��������ƶ�
                        auto moveKey = std::make_tuple(i, j, ni, nj);
                        if (recentMoves.find(moveKey) != recentMoves.end()) continue;

                        // ֻ���ǰ�ȫĿ�� (�ɼ��ұ���ռ��)
                        if (gridType[ni][nj] != '?' &&
                            gridArmy[i][j] / 2 > gridArmy[ni][nj] * 1.2) {

                            possibleMoves.push_back({i, j, d});
                        }
                    }
                }
            }
        }

        if (!possibleMoves.empty()) {
            // ���ѡ������������
            std::uniform_int_distribution<int> dist(0, possibleMoves.size()-1);
            int idx = dist(rng);
            auto [x, y, d] = possibleMoves[idx];

            // ��¼�ƶ�
            int nx = x + dx[d];
            int ny = y + dy[d];
            recentMoves.insert(std::make_tuple(x, y, nx, ny));
            if (recentMoves.size() > 10) recentMoves.erase(recentMoves.begin());

            movement m;
            m.x = x;
            m.y = y;
            m.dir = d;
            return m;
        }

        // ��ȫ���ƶ�
        movement invalid;
        invalid.dir = 4;
        return invalid;
    }

    // ���������ƶ������ĺ�����
    movement lightningExpand() {
        // ����1: ʹ�����п��ñ����������� (1-indexedѭ��)
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // ֻ�������ɼ�����
                if (gridType[i][j] != '?' && gridBelong[i][j] == selfId && gridArmy[i][j] > 1) {
                    // �������������ڱ�����
                    if (earlyGame && i == generalX && j == generalY && gridArmy[i][j] < 10) continue;

                    // �������з�������
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dx[d];
                        int nj = j + dy[d];

                        // 1-indexed�߽���
                        if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                        // ������Ч����
                        if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                            gridType[ni][nj] == 'B') continue;

                        // ֻ����ɼ�����������
                        if (gridType[ni][nj] == '?') continue;

                        // ������������
                        if (gridBelong[ni][nj] == 0) {
                            // ȷ���������ƣ����ڽ���Ҫ��
                            bool canCapture = false;
                            if (earlyGame) {
                                canCapture = gridArmy[i][j] / 2 > gridArmy[ni][nj] * 0.8;
                            } else {
                                canCapture = gridArmy[i][j] / 2 > gridArmy[ni][nj];
                            }

                            if (canCapture) {
                                movement m;
                                m.x = i;
                                m.y = j;
                                m.dir = d;
                                return m;
                            }
                        }
                    }
                }
            }
        }

        // ����2: ʹ��BFS�������ŵ��߽�
        for (auto& frontier : expansionFrontiers) {
            auto move = findFastestPathToBorder(frontier.first, frontier.second);
            if (move.dir < 4) return move;
        }

        // ����3: ̽��δ֪����
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                if (gridType[i][j] == '?') {
                    // Ѱ������ļ�����λ (1-indexed)
                    int bestX = -1, bestY = -1;
                    int bestDist = INT_MAX;

                    for (int x = 1; x <= mapRows; x++) {
                        for (int y = 1; y <= mapCols; y++) {
                            if (gridType[x][y] != '?' && gridBelong[x][y] == selfId && gridArmy[x][y] > 1) {
                                int dist = abs(x - i) + abs(y - j);
                                if (dist < bestDist) {
                                    bestDist = dist;
                                    bestX = x;
                                    bestY = y;
                                }
                            }
                        }
                    }

                    if (bestX != -1) {
                        // ֱ����δ֪�����ƶ�
                        int bestDir = -1;
                        int minDist = INT_MAX;

                        for (int d = 0; d < 4; d++) {
                            int ni = bestX + dx[d];
                            int nj = bestY + dy[d];

                            // 1-indexed�߽���
                            if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                            int dist = abs(ni - i) + abs(nj - j);
                            if (dist < minDist) {
                                minDist = dist;
                                bestDir = d;
                            }
                        }

                        if (bestDir != -1) {
                            movement m;
                            m.x = bestX;
                            m.y = bestY;
                            m.dir = bestDir;
                            return m;
                        }
                    }
                }
            }
        }

        // ����4: ������ȫ����
        if (generalX != -1 && generalY != -1 &&
            gridType[generalX][generalY] != '?' &&
            gridArmy[generalX][generalY] > 1) {

            // Ѱ��������ŷ���
            int bestDir = -1;
            int bestScore = INT_MIN;

            for (int d = 0; d < 4; d++) {
                int ni = generalX + dx[d];
                int nj = generalY + dy[d];

                // 1-indexed�߽���
                if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                // ������Ч����
                if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                    gridType[ni][nj] == 'B') continue;

                // �������ŷ���
                int score = 0;

                // �������ظ߷�
                if (gridType[ni][nj] != '?' && gridBelong[ni][nj] == 0) score += 1000;

                // δ֪����߷�
                if (gridType[ni][nj] == '?') score += 800;

                // ��ȫ����
                if (gridType[ni][nj] != '?' &&
                    gridArmy[generalX][generalY] / 2 > gridArmy[ni][nj] * 1.5) {
                    score += 300;
                }

                if (score > bestScore) {
                    bestScore = score;
                    bestDir = d;
                }
            }

            if (bestDir != -1) {
                movement m;
                m.x = generalX;
                m.y = generalY;
                m.dir = bestDir;
                return m;
            }
        }

        // ����5: �����ȫ����
        return randomSafeExpand();
    }

    // ���ƶ����� (1-indexed)
    movement Move() {
        turnCount++;

        // �����Ϸ�׶�
        earlyGame = (turnCount < 50);

        // 1-indexed��ͼ����
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                block b = Ask(i, j); // ʹ��1-indexed����
                gridType[i][j] = b.type;
                gridArmy[i][j] = b.army;
                gridBelong[i][j] = b.belongTo; // ��ȷ����belongTo

                // ȷ������ID (ֻ�ӿɼ���Ч����)
                if (selfId == -1 && b.type != '?' && b.belongTo > 0) {
                    selfId = b.belongTo;
                }

                // ��⽫�� (ֻ�ӿɼ���Ч����)
                if (b.type == 'K' && b.belongTo == selfId) {
                    generalX = i;
                    generalY = j;
                }
            }
        }

        // �������ű߽�
        updateExpansionFrontiers();

        // �������Ų���
        return lightningExpand();
    }
}
