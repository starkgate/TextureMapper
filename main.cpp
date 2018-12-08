#include "main.h"
#include "utility_csv.h"
#include "utility_sqlite.h"
#include "ui_mainwindow.h"
#include "utility_logger.h"

void initialize() {
    log_init();
    sqlite_init();
}

void finish() {
    sqlite_term();
    log_term();
}

/*
def setup
  return unless internet?
  # initialize trees if they don't exist
  (1..3).each { |g| create_tree g } if File.empty? 'full.db'

  # update from github
  update_texture_map
end

# if database doesn't exist, create it
# else if database exists, check if texture map has been modified
# if yes, download the new and replace the original
def update_texture_map
  if File.empty? 'database.db'
    download_texture_map
    create_db
  elsif tree_updated?
    download_texture_map
    update_db
  end
  File.delete @texture_map if File.exist? @texture_map
end

def tree_updated?
  last_modified_time = File.open('database.db').mtime.strftime('%FT%TZ')
  HTTP.get("https://api.github.com/repos/CreeperLava/TextureMapperCrystal/commits?path=#{@texture_map}&since=#{last_modified_time}").to_s != '[]'
end

def download_texture_map
  File.write(@texture_map, HTTP.get('https://raw.githubusercontent.com/CreeperLava/TextureMapperCrystal/master/Texture_Map.csv').to_s.gsub("\r\n", "\n"))
end

def download_game_map(game)
  File.write("ME#{game}_Tree.csv", HTTP.get("https://raw.githubusercontent.com/CreeperLava/TextureMapperCrystal/master/ME#{game}_Tree.csv").to_s.gsub("\r\n", "\n"))
end
 */

void create_tree(int game) {
    /*
    def create_tree(game)
      download_game_map(game)

      @full_db.execute <<-SQL
        create table ME#{game} (
          crc varchar(10),
          name varchar(100),
          PRIMARY KEY(crc, name)
        );
      SQL

      @full_db.transaction
      CSV.foreach("ME#{game}.csv", headers: true) do |row|
        @full_db.execute("insert into ME#{game} values ( ?, ? )", row)
      @full_db.commit
      end

      # add index for faster searches
      @full_db.execute("create index index_crc_me#{game} on ME#{game} (crc)")
      @full_db.execute('vacuum')
      File.delete("ME#{game}_Tree.csv")
    end
     */
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    finish();
    delete ui;
}

void MyThread::run() {
    log("Started initialization thread...");
    initialize();
    log("Initialized Texture Mapper successfully !");
    // TODO progress bar if GUI starts before DB and DB isn't ready ?
}

int qt_init(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}

int main(int argc, char *argv[]) {
    MyThread thread_init;

    qt_init(argc, argv); // main thread, UI
    thread_init.start(); // separate thread for database creation / update

    return 0;
}
