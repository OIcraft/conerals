namespace DeepSeek {

    // 全局状态 (1-indexed)
    int selfId = -1;
    int generalX = -1;
    int generalY = -1;
    int turnCount = 0;

    // 闪电扩张状态
    bool earlyGame = true;
    std::vector<std::pair<int, int>> expansionFrontiers;
    int lastFrontierUpdate = -10;

    // 高级地图状态 (1-indexed)
    std::vector<std::vector<char>> gridType;
    std::vector<std::vector<int>> gridArmy;
    std::vector<std::vector<int>> gridBelong;

    // 方向数组
    const int dx[4] = {0, -1, 0, 1}; // 右、上、左、下
    const int dy[4] = {1, 0, -1, 0};

    // 移动历史（防止振荡）
    std::set<std::tuple<int, int, int, int>> recentMoves; // (fromX, fromY, toX, toY)

    // 随机数生成器
    std::mt19937 rng;

    // 初始化函数
    void Init() {
        selfId = -1;
        generalX = -1;
        generalY = -1;
        turnCount = 0;
        earlyGame = true;
        expansionFrontiers.clear();
        recentMoves.clear();
        rng.seed(time(0));

        // 1-indexed数组初始化 (+1大小)
        gridType.resize(mapRows + 1, std::vector<char>(mapCols + 1, '?'));
        gridArmy.resize(mapRows + 1, std::vector<int>(mapCols + 1, 0));
        gridBelong.resize(mapRows + 1, std::vector<int>(mapCols + 1, -1)); // 默认不可见
    }

    // 极速扩张评估
    void updateExpansionFrontiers() {
        // 每5回合更新一次边界
        if (turnCount - lastFrontierUpdate < 5) return;
        lastFrontierUpdate = turnCount;
        expansionFrontiers.clear();

        // 1-indexed循环
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // 跳过不可见和敌方格子
                if (gridType[i][j] == '?' || gridBelong[i][j] != selfId) continue;

                bool hasExpandable = false;

                // 检查所有相邻格子
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d];
                    int nj = j + dy[d];

                    // 边界检查 (1-indexed)
                    if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                    // 有效扩张目标：中立(0)或未知(?)
                    if (gridBelong[ni][nj] == 0 || gridType[ni][nj] == '?') {
                        hasExpandable = true;
                        break;
                    }
                }

                // 添加到扩张边界
                if (hasExpandable) {
                    expansionFrontiers.push_back({i, j});
                }
            }
        }

        // 如果没有边界，使用将军位置
        if (expansionFrontiers.empty() && generalX != -1) {
            expansionFrontiers.push_back({generalX, generalY});
        }
    }

    // 超快速路径规划
    movement findFastestPathToBorder(int startX, int startY) {
        // BFS寻找最近边界
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

            // 检查是否边界
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                // 1-indexed边界检查
                if (nx < 1 || nx > mapRows || ny < 1 || ny > mapCols) continue;

                // 发现可扩张区域：中立或未知
                if (gridBelong[nx][ny] == 0 || gridType[nx][ny] == '?') {
                    // 回溯到第一步
                    int firstX = startX, firstY = startY;
                    int cx = x, cy = y;

                    // 回溯路径找到第一步
                    while (parent[cx][cy].first != startX || parent[cx][cy].second != startY) {
                        auto [px, py] = parent[cx][cy];
                        cx = px;
                        cy = py;
                        firstX = cx;
                        firstY = cy;
                    }

                    // 确定方向
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

            // 扩展邻居
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                // 1-indexed边界检查
                if (nx < 1 || nx > mapRows || ny < 1 || ny > mapCols) continue;
                if (visited[nx][ny]) continue;

                // 只能通过己方或安全区域
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

    // 随机安全扩张 (1-indexed)
    movement randomSafeExpand() {
        std::vector<std::tuple<int, int, int>> possibleMoves;

        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // 只处理可见己方格子
                if (gridType[i][j] != '?' && gridBelong[i][j] == selfId && gridArmy[i][j] > 1) {
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dx[d];
                        int nj = j + dy[d];

                        // 1-indexed边界检查
                        if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                        // 跳过无效格子
                        if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                            gridType[ni][nj] == 'B' || gridType[ni][nj] == '?') continue;

                        // 跳过最近移动
                        auto moveKey = std::make_tuple(i, j, ni, nj);
                        if (recentMoves.find(moveKey) != recentMoves.end()) continue;

                        // 只考虑安全目标 (可见且兵力占优)
                        if (gridType[ni][nj] != '?' &&
                            gridArmy[i][j] / 2 > gridArmy[ni][nj] * 1.2) {

                            possibleMoves.push_back({i, j, d});
                        }
                    }
                }
            }
        }

        if (!possibleMoves.empty()) {
            // 随机选择但优先新区域
            std::uniform_int_distribution<int> dist(0, possibleMoves.size()-1);
            int idx = dist(rng);
            auto [x, y, d] = possibleMoves[idx];

            // 记录移动
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

        // 完全无移动
        movement invalid;
        invalid.dir = 4;
        return invalid;
    }

    // 闪电扩张移动（核心函数）
    movement lightningExpand() {
        // 策略1: 使用所有可用兵力立即扩张 (1-indexed循环)
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                // 只处理己方可见格子
                if (gridType[i][j] != '?' && gridBelong[i][j] == selfId && gridArmy[i][j] > 1) {
                    // 跳过将军（早期保护）
                    if (earlyGame && i == generalX && j == generalY && gridArmy[i][j] < 10) continue;

                    // 尝试所有方向扩张
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dx[d];
                        int nj = j + dy[d];

                        // 1-indexed边界检查
                        if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                        // 跳过无效格子
                        if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                            gridType[ni][nj] == 'B') continue;

                        // 只处理可见或中立区域
                        if (gridType[ni][nj] == '?') continue;

                        // 优先中立土地
                        if (gridBelong[ni][nj] == 0) {
                            // 确保兵力优势（早期降低要求）
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

        // 策略2: 使用BFS快速扩张到边界
        for (auto& frontier : expansionFrontiers) {
            auto move = findFastestPathToBorder(frontier.first, frontier.second);
            if (move.dir < 4) return move;
        }

        // 策略3: 探索未知区域
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                if (gridType[i][j] == '?') {
                    // 寻找最近的己方单位 (1-indexed)
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
                        // 直接向未知区域移动
                        int bestDir = -1;
                        int minDist = INT_MAX;

                        for (int d = 0; d < 4; d++) {
                            int ni = bestX + dx[d];
                            int nj = bestY + dy[d];

                            // 1-indexed边界检查
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

        // 策略4: 将军安全扩张
        if (generalX != -1 && generalY != -1 &&
            gridType[generalX][generalY] != '?' &&
            gridArmy[generalX][generalY] > 1) {

            // 寻找最佳扩张方向
            int bestDir = -1;
            int bestScore = INT_MIN;

            for (int d = 0; d < 4; d++) {
                int ni = generalX + dx[d];
                int nj = generalY + dy[d];

                // 1-indexed边界检查
                if (ni < 1 || ni > mapRows || nj < 1 || nj > mapCols) continue;

                // 跳过无效格子
                if (gridType[ni][nj] == 'x' || gridType[ni][nj] == 'M' ||
                    gridType[ni][nj] == 'B') continue;

                // 计算扩张分数
                int score = 0;

                // 中立土地高分
                if (gridType[ni][nj] != '?' && gridBelong[ni][nj] == 0) score += 1000;

                // 未知区域高分
                if (gridType[ni][nj] == '?') score += 800;

                // 安全考虑
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

        // 策略5: 随机安全扩张
        return randomSafeExpand();
    }

    // 主移动函数 (1-indexed)
    movement Move() {
        turnCount++;

        // 检测游戏阶段
        earlyGame = (turnCount < 50);

        // 1-indexed地图更新
        for (int i = 1; i <= mapRows; i++) {
            for (int j = 1; j <= mapCols; j++) {
                block b = Ask(i, j); // 使用1-indexed坐标
                gridType[i][j] = b.type;
                gridArmy[i][j] = b.army;
                gridBelong[i][j] = b.belongTo; // 正确处理belongTo

                // 确定自身ID (只从可见有效格子)
                if (selfId == -1 && b.type != '?' && b.belongTo > 0) {
                    selfId = b.belongTo;
                }

                // 检测将军 (只从可见有效格子)
                if (b.type == 'K' && b.belongTo == selfId) {
                    generalX = i;
                    generalY = j;
                }
            }
        }

        // 更新扩张边界
        updateExpansionFrontiers();

        // 闪电扩张策略
        return lightningExpand();
    }
}
