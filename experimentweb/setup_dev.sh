rm -rf db.sqlite3
rm -rf media/
find . -path "*/migrations/*.py" -not -name "__init__.py" -delete
find . -path "*/migrations/*.pyc"  -delete

python manage.py makemigrations
python manage.py migrate
python manage.py shell < init_db.py
python manage.py runserver 0.0.0.0:8000 