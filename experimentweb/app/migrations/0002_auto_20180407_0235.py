# Generated by Django 2.0.3 on 2018-04-06 19:35

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('app', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='experimentresult',
            name='a',
            field=models.IntegerField(default=50),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='cell',
            field=models.IntegerField(default=130),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='e',
            field=models.IntegerField(default=10),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='loop',
            field=models.IntegerField(default=3),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='pw',
            field=models.IntegerField(default=50),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='t',
            field=models.IntegerField(default=100),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='vcc',
            field=models.IntegerField(default=3300),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='vmax',
            field=models.IntegerField(default=800),
        ),
        migrations.AddField(
            model_name='experimentresult',
            name='vmin',
            field=models.IntegerField(default=0),
        ),
    ]
