# Generated by Django 2.0.3 on 2018-03-16 08:00

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='ExperimentResult',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('create_time', models.DateTimeField(auto_now_add=True)),
                ('i', models.CharField(max_length=3000)),
                ('v', models.CharField(max_length=3000)),
            ],
        ),
    ]
